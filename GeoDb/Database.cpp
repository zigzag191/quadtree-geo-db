#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

#include "Database.h"

#include "osmium/io/xml_input.hpp"
#include "osmium/handler.hpp"
#include "osmium/visitor.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/way.hpp"

namespace
{
	constexpr auto Pi = 3.14;
	constexpr auto QuaterPi = Pi / 4;
	constexpr auto DefaultR = 10000.0;
	constexpr auto L0 = 0.0;

	inline double ProjectLongitude(double longitude, double r = DefaultR)
	{
		return r * ((longitude * Pi / 180) - L0);
	}

	inline double ProjectLattitude(double lattitude, double r = DefaultR)
	{
		return r * std::log(std::tan(QuaterPi + ((lattitude * Pi / 180) / 2)));
	}

	quadtree::Rectangle<double> GetMapArea(const geodb::Map& map)
	{
		const auto& nodes = map.GetNodes();

		auto maxX = nodes[0].GetX();
		auto maxY = nodes[0].GetY();
		auto minX = nodes[0].GetX();
		auto minY = nodes[0].GetY();

		for (const auto& node : nodes)
		{
			maxX = std::max(maxX, node.GetX());
			maxY = std::max(maxY, node.GetY());
			minX = std::min(minX, node.GetX());
			minY = std::min(minY, node.GetY());
		}

		return quadtree::Rectangle<double>::Of(minX, maxY, maxX - minX, maxY - minY);
	}

	class MapImportingHandler : public osmium::handler::Handler
	{
	public:
		void way(const osmium::Way& way)
		{
			auto wayNodeIds = std::vector<std::size_t>{};
			wayNodeIds.reserve(way.nodes().size());

			for (const auto& nodeReference : way.nodes())
			{
				wayNodeIds.push_back(nodeOsmIdToMapId[nodeReference.ref()]);
			}

			map.GetWays().emplace_back(wayNodeIds);
		}

		void node(const osmium::Node& node)
		{
			map.GetNodes().emplace_back(
				ProjectLongitude(node.location().lon()),
				ProjectLattitude(node.location().lat())
			);
			nodeOsmIdToMapId[node.id()] = map.GetNodes().size() - 1;
		}

		geodb::Map GetMap()
		{
			return std::move(map);
		}

	private:
		using OsmId = osmium::object_id_type;
		using MapId = std::size_t;

		geodb::Map map;
		std::unordered_map<OsmId, MapId> nodeOsmIdToMapId;
	};
}

namespace geodb
{
	Database Database::FromFile(std::string_view osmFileName)
	{
		auto reader = osmium::io::Reader{ osmFileName.data() };
		auto handler = MapImportingHandler{};
		osmium::apply(reader, handler);
		return Database{ handler.GetMap() };
	}

	Database::Database(Map map)
		: m_map{ std::move(map) }
		, m_quadtree{ GetMapArea(m_map), QuadtreeMaxDepth}
	{
		for (std::size_t i = 0; i < map.GetWays().size(); ++i)
		{
			m_quadtree.Insert(m_map.GetWayBoundingBox(i));
		}
	}
}
