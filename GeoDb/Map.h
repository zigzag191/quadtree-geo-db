#pragma once

#include <vector>

#include "BoundingBox.h"

namespace geodb
{
	class Node
	{
	public:
		Node(double x, double y)
			: m_x{ x }
			, m_y{ y }
		{ }

		double GetX() const { return m_x; }

		double GetY() const { return m_y; }

	private:
		double m_x;
		double m_y;
	};

	class Way
	{
	public:
		Way(std::vector<std::size_t> nodes)
			: m_nodes{ std::move(nodes) }
		{ }

		const std::vector<std::size_t>& GetNodes() const { return m_nodes; }

	private:
		std::vector<std::size_t> m_nodes;
	};

	class Map
	{
	public:
		std::vector<Node>& GetNodes() { return m_nodes; }

		std::vector<Way>& GetWays() { return m_ways; }

		const std::vector<Node>& GetNodes() const { return m_nodes; }

		const std::vector<Way>& GetWays() const { return m_ways; }

		geodb::BoundingBox GetWayBoundingBox(std::size_t objectIndex)
		{
			const auto& way = m_ways[objectIndex];
			auto maxX = m_nodes[way.GetNodes()[0]].GetX();
			auto maxY = m_nodes[way.GetNodes()[0]].GetY();
			auto minX = m_nodes[way.GetNodes()[0]].GetX();
			auto minY = m_nodes[way.GetNodes()[0]].GetY();

			for (const auto& node : way.GetNodes())
			{
				maxX = std::max(maxX, m_nodes[node].GetX());
				maxY = std::max(maxY, m_nodes[node].GetY());
				minX = std::min(minX, m_nodes[node].GetX());
				minY = std::min(minY, m_nodes[node].GetY());
			}

			return geodb::BoundingBox{ objectIndex, minX, maxY, maxX - minX, maxY - minY };
		}

	private:
		std::vector<Node> m_nodes;
		std::vector<Way> m_ways;
	};
}
