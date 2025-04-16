#pragma once

#include <vector>
#include <string_view>

#include "BoundingBox.h"
#include "Map.h"
#include "ObjectType.h"
#include "../Quadtree/Quadtree.h"

namespace geodb
{
	class Database
	{
	public:
		struct QueryResult
		{
			std::size_t objectId;
			ObjectType objectType;
		};

	public:
		static Database FromFile(std::string_view osmFileName);

		Database(const Database& other) = delete;

		Database& operator=(const Database& other) = delete;

		const Map& GetMap() const { return m_map; }

		const quadtree::Rectangle<double>& GetIndexedArea() const { return m_quadtree.GetIndexedArea(); }

		std::vector<std::size_t> Query(const quadtree::Rectangle<double>& searchWindow);

	private:
		Database(Map map);

	private:
		static constexpr int QuadtreeMaxDepth = 10;

		Map m_map;
		quadtree::Quadtree<double, BoundngBox> m_quadtree;
	};
}
