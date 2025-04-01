#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

#include <iostream>

#include "../OSMImporter/Osm.h"
#include "../Quadtree/Quadtree.h"
#include "../Quadtree/Rectangle.h"

namespace
{
	struct BoundingBox : public quadtree::Rectangle<double>
	{
		BoundingBox(std::size_t objectIndex, double topLeftX, double topLeftY, double width, double height)
			: Rectangle{ topLeftX + width / 2, topLeftY - height / 2, width / 2, height / 2 }
			, objectIndex{ objectIndex }
		{}

		BoundingBox() {}

		std::size_t objectIndex;
	};

	quadtree::Rectangle<double> GetMapArea(const osmimporter::Map& map)
	{
		auto maxX = map.nodes[0].x;
		auto maxY = map.nodes[0].y;
		auto minX = map.nodes[0].x;
		auto minY = map.nodes[0].y;
		for (const auto& node : map.nodes)
		{
			maxX = std::max(maxX, node.x);
			maxY = std::max(maxY, node.y);
			minX = std::min(minX, node.x);
			minY = std::min(minY, node.y);
		}
		return quadtree::Rectangle<double>::Of(minX, maxY, maxX - minX, maxY - minY);
	}

	BoundingBox GetWayArea(std::size_t objectIndex, const osmimporter::Map& map)
	{
		const auto& way = map.ways[objectIndex];
		auto maxX = map.nodes[way.nodes[0]].x;
		auto maxY = map.nodes[way.nodes[0]].y;
		auto minX = map.nodes[way.nodes[0]].x;
		auto minY = map.nodes[way.nodes[0]].y;
		for (const auto& node : way.nodes)
		{
			maxX = std::max(maxX, map.nodes[node].x);
			maxY = std::max(maxY, map.nodes[node].y);
			minX = std::min(minX, map.nodes[node].x);
			minY = std::min(minY, map.nodes[node].y);
		}
		return BoundingBox{ objectIndex, minX, maxY, maxX - minX, maxY - minY };
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Osm file path in xml formal must be passed as the only argument\n";
		return EXIT_FAILURE;
	}

	const auto fileName = argv[1];
	const auto map = osmimporter::MapImporter::ReadFile(fileName);

	static constexpr auto maxQuadtreeDepth = 10;
	auto quadtree = quadtree::Quadtree<double, BoundingBox>{ GetMapArea(map), maxQuadtreeDepth };

	for (std::size_t i = 0; i < map.ways.size(); ++i)
	{
		quadtree.Insert(GetWayArea(i, map));
	}

	return EXIT_SUCCESS;
}
