#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

#include <iostream>
#include <memory>
#include <unordered_map>

#include "osmium/io/xml_input.hpp"
#include "osmium/handler.hpp"
#include "osmium/visitor.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/way.hpp"

#include "Osm.h"

namespace
{
	class MapImportingHandler : public osmium::handler::Handler
	{
	public:
		MapImportingHandler(std::shared_ptr<Map> map)
			: map{ map }
		{ }

		void way(const osmium::Way& way)
		{
			Way mapWay;
			for (const auto& nodeReference : way.nodes())
			{
				mapWay.nodes.push_back(nodeOsmIdToMapId[nodeReference.ref()]);
			}
			map->ways.push_back(std::move(mapWay));
		}

		void node(const osmium::Node& node)
		{
			map->nodes.push_back(Node{ node.location().lon(), node.location().lat() });
			nodeOsmIdToMapId[node.id()] = map->nodes.size() - 1;
		}

	private:
		std::shared_ptr<Map> map;

		using OsmId = osmium::object_id_type;
		using MapId = std::size_t;

		std::unordered_map<OsmId, MapId> nodeOsmIdToMapId;
	};

	void PrintNode(const Node& node)
	{
		std::cout << "Node[lon: " << node.longitude << ", lat: " << node.latitude << "]";
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Map file in osm xml format should be passed as the only parameter";
		return EXIT_FAILURE;
	}

	const auto mapFilePath = argv[1];
	auto reader = osmium::io::Reader{ mapFilePath };

	auto map = std::make_shared<Map>();
	
	auto handler = MapImportingHandler{ map };
	osmium::apply(reader, handler);

	for (const auto& way : map->ways)
	{
		std::cout << "Way[from: ";
		PrintNode(map->nodes[way.nodes.front()]);
		std::cout << ", to: ";
		PrintNode(map->nodes[way.nodes.back()]);
		std::cout << "]\n";
	}

	return EXIT_SUCCESS;
}
