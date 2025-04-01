#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string_view>

#include "osmium/io/xml_input.hpp"
#include "osmium/handler.hpp"
#include "osmium/visitor.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/way.hpp"

namespace osmimporter
{
	struct Node
	{
		double x;
		double y;
	};

	struct Way
	{
		std::vector<std::size_t> nodes;
	};

	struct Map
	{
		std::vector<Node> nodes;
		std::vector<Way> ways;
	};

	class MapImporter
	{
	public:
		static Map ReadFile(std::string_view fileName)
		{
			auto reader = osmium::io::Reader{ fileName.data() };
			auto handler = MapImportingHandler{};
			osmium::apply(reader, handler);
			return handler.GetMap();
		}

		MapImporter() = delete;

	private:
		class MapImportingHandler : public osmium::handler::Handler
		{
		public:
			void way(const osmium::Way& way)
			{
				map.ways.push_back(Way{});
				auto& mapWay = map.ways[map.ways.size() - 1];
				for (const auto& nodeReference : way.nodes())
				{
					mapWay.nodes.push_back(nodeOsmIdToMapId[nodeReference.ref()]);
				}
			}

			void node(const osmium::Node& node)
			{
				map.nodes.push_back(Project(node));
				nodeOsmIdToMapId[node.id()] = map.nodes.size() - 1;
			}

			Map GetMap()
			{
				return std::move(map);
			}

		private:
			Node Project(const osmium::Node& node)
			{
				static constexpr auto Pi = 3.14;
				static constexpr auto QuaderPi = Pi / 4;
				static constexpr auto R = 10000.0;
				static constexpr auto l0 = 0.0;

				const auto l = node.location().lon() * Pi / 180;
				const auto phi = node.location().lat() * Pi / 180;

				return Node{ R * (l - l0), R * std::log(std::tan(QuaderPi + (phi / 2))) };
			}

		private:
			using OsmId = osmium::object_id_type;
			using MapId = std::size_t;

			Map map;
			std::unordered_map<OsmId, MapId> nodeOsmIdToMapId;
		};
	};
}
