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

#include "Algo2d.h"

namespace
{
	constexpr auto Pi = 3.14;
	constexpr auto QuaterPi = Pi / 4.0;
	constexpr auto DefaultR = 10000.0;
	constexpr auto L0 = 0.0;

	inline double ProjectLongitude(double longitude, double r = DefaultR)
	{
		return r * ((longitude * Pi / 180.0) - L0);
	}

	inline double ProjectLattitude(double lattitude, double r = DefaultR)
	{
		auto d = 10000.0 * log(tan((3.14 / 4.0) +
			((lattitude * 3.14 / 180.0) / 2.0)));
		return r * std::log(std::tan(QuaterPi +
			                        ((lattitude * Pi / 180.0) / 2.0)));
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

	quadtree::Rectangle<double> GetWayBoundingBox(const std::vector<std::size_t>& wayNodeIds, const geodb::Map& map)
	{
		const auto& nodes = map.GetNodes();

		auto maxX = nodes[wayNodeIds[0]].GetX();
		auto maxY = nodes[wayNodeIds[0]].GetY();
		auto minX = nodes[wayNodeIds[0]].GetX();
		auto minY = nodes[wayNodeIds[0]].GetY();

		for (const auto& node : wayNodeIds)
		{
			maxX = std::max(maxX, nodes[node].GetX());
			maxY = std::max(maxY, nodes[node].GetY());
			minX = std::min(minX, nodes[node].GetX());
			minY = std::min(minY, nodes[node].GetY());
		}

		return quadtree::Rectangle<double>::Of(minX, maxY, maxX - minX, maxY - minY);
	}

	bool Contains(const quadtree::Rectangle<double>& a, const quadtree::Rectangle<double>& b)
	{
		return a.GetCenterX() - a.GetHalfWidth() <= b.GetCenterX() - b.GetHalfWidth() &&
			   a.GetCenterX() + a.GetHalfWidth() >= b.GetCenterX() + b.GetHalfWidth() &&
			   a.GetCenterY() + a.GetHalfHeight() >= b.GetCenterY() + b.GetHalfHeight() &&
			   a.GetCenterY() - a.GetHalfHeight() <= b.GetCenterY() - b.GetHalfHeight();
	}

	bool Contains(const geodb::Node& node, const quadtree::Rectangle<double>& searchWindow)
	{
		return node.GetX() >= searchWindow.GetCenterX() - searchWindow.GetHalfWidth() &&
			   node.GetX() <= searchWindow.GetCenterX() + searchWindow.GetHalfWidth() &&
			   node.GetY() <= searchWindow.GetCenterY() + searchWindow.GetHalfHeight() &&
			   node.GetY() >= searchWindow.GetCenterY() - searchWindow.GetHalfHeight();
	}

	bool Intersects(const geodb::Way& way, const quadtree::Rectangle<double>& searchWindow, const geodb::Map& map)
	{
		if (Contains(searchWindow, way.GetBoundingBox()))
		{
			return true;
		}

		const auto& nodes = map.GetNodes();

		for (std::size_t i = 0; i < way.GetNodes().size() - 1; ++i)
		{
			const auto lineIntersectsWindow = geodb::algo::LineRectangleIntersection(
				nodes[i].GetX(), nodes[i].GetY(), nodes[i + 1].GetX(), nodes[i + 1].GetY(),
				searchWindow.GetCenterX() - searchWindow.GetHalfWidth(), searchWindow.GetCenterY() + searchWindow.GetHalfHeight(),
				searchWindow.GetHalfWidth() * 2.0, searchWindow.GetHalfHeight() * 2
			);
			if (lineIntersectsWindow)
			{
				return true;
			}
		}

		return false;
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

			map.GetWays().emplace_back(wayNodeIds, GetWayBoundingBox(wayNodeIds, map));

			const auto wayId = map.GetWays().size() - 1;

			for (const auto& tag : way.tags())
			{
				map.AddTagToObject(wayId, tag.key(), tag.value());
			}
		}

		void node(const osmium::Node& node)
		{
			map.GetNodes().emplace_back(
				ProjectLongitude(node.location().lon()),
				ProjectLattitude(node.location().lat())
			);

			const auto nodeId = map.GetNodes().size() - 1;

			for (const auto& tag : node.tags())
			{
				map.AddTagToObject(nodeId, tag.key(), tag.value());
			}

			nodeOsmIdToMapId[node.id()] = nodeId;
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

	std::vector<std::size_t> Database::Query(const quadtree::Rectangle<double>& searchWindow)
	{
		const auto candidates = m_quadtree.Query(searchWindow);

		auto result = std::vector<std::size_t>{};
		for (const auto candidate : candidates)
		{
			switch (candidate->GetObjectType())
			{
			case ObjectType::Way:
			{
				if (Intersects(m_map.GetWays()[candidate->GetObjectIndex()], searchWindow, m_map))
				{
					result.push_back(candidate->GetObjectIndex());
				}
				break;
			}
			case ObjectType::Node:
			{
				if (Contains(m_map.GetNodes()[candidate->GetObjectIndex()], searchWindow))
				{
					result.push_back(candidate->GetObjectIndex());
				}
				break;
			}
			}
		}

		return result;
	}

	Database::Database(Map map)
		: m_map{ std::move(map) }
		, m_quadtree{ GetMapArea(m_map), QuadtreeMaxDepth}
	{
		for (std::size_t i = 0; i < m_map.GetWays().size(); ++i)
		{
			m_quadtree.Insert(BoundngBox{ i, ObjectType::Way, m_map.GetWays()[i].GetBoundingBox() });
		}
		for (std::size_t i = 0; i < m_map.GetNodes().size(); ++i)
		{
			if (m_map.GetObjectTags(i) != nullptr)
			{
				const auto& node = m_map.GetNodes()[i];
				m_quadtree.Insert(BoundngBox{ i, ObjectType::Node, quadtree::Rectangle{ node.GetX(), node.GetY(), DefaultR / 2.0, DefaultR / 2.0 }});
			}
		}
	}
}
