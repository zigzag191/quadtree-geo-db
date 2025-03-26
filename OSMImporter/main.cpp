#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

#include <iostream>

#include "osmium/io/xml_input.hpp"
#include "osmium/handler.hpp"
#include "osmium/visitor.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/way.hpp"

class PrintHandler : public osmium::handler::Handler
{
public:
	void way(const osmium::Way& way) const
	{
		std::cout << "Way[from: ";
		PrintNode(way.nodes().front().location());
		std::cout << ", to: ";
		PrintNode(way.nodes().back().location());
		std::cout << "]\n";
	}

private:
	void PrintNode(const osmium::Location& location) const
	{
		std::cout << "Node[lon: " << location.lon() << ", lat: " << location.lat() << "]";
	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Map file in osm xml format should be passed as the only parameter";
		return EXIT_FAILURE;
	}

	const auto mapFilePath = argv[1];

	auto reader = osmium::io::Reader{ mapFilePath };
	const auto printHandler = PrintHandler{};

	osmium::apply(reader, printHandler);

	return EXIT_SUCCESS;
}
