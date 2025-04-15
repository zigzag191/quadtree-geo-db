#pragma once

#include <vector>

#include "../Quadtree/Rectangle.h"

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
		Way(std::vector<std::size_t> nodes, const quadtree::Rectangle<double>& boundingBox)
			: m_boundingBox{ boundingBox }
			, m_nodes{ std::move(nodes) }
		{ }

		const std::vector<std::size_t>& GetNodes() const { return m_nodes; }

		const quadtree::Rectangle<double>& GetBoundingBox() const { return m_boundingBox; };

	private:
		quadtree::Rectangle<double> m_boundingBox;
		std::vector<std::size_t> m_nodes;
	};

	class Map
	{
	public:
		std::vector<Node>& GetNodes() { return m_nodes; }

		std::vector<Way>& GetWays() { return m_ways; }

		const std::vector<Node>& GetNodes() const { return m_nodes; }

		const std::vector<Way>& GetWays() const { return m_ways; }

	private:
		std::vector<Node> m_nodes;
		std::vector<Way> m_ways;
	};
}
