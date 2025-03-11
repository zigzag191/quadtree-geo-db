#pragma once

#include <concepts>

#include "Common.h"
#include "FreeList.h"
#include "Rectangle.h"

template<typename T, typename N>
concept Rectangular = Numeric<N> && requires(T a)
{
	{ a.GetCenterX() } -> std::convertible_to<N>;
	{ a.GetCenterY() } -> std::convertible_to<N>;
	{ a.GetHalfWidth() } -> std::convertible_to<N>;
	{ a.GetHalfHeight() } -> std::convertible_to<N>;
};

template<Numeric N, Rectangular<N> R>
class Quadtree
{
public:
	Quadtree(const Rectangle<N>& indexedArea, Index maxDepth)
		: indexedArea{ indexedArea }
		, maxDepth{ maxDepth }
	{
		root = quadtreeNodes.Insert();
	}

	void Insert(const R& r)
	{
		root.Insert(indexedArea, r, 1);
	}

private:
	enum struct Quadrant
	{
		SW = 0,
		SE = 1,
		NE = 2,
		NW = 3
	};

	struct QuadtreeNode
	{
		Index children[4] = { -1, -1, -1, -1 };
		Index xAxis;
		Index yAxis;
	};

	struct LinkedListNode
	{
		R element;
		Index next = -1;
	};

	struct AxisBinaryTreeNode
	{
		Index left = -1;
		Index right = -1;
		Index firstElement = -1;
	};

	enum struct AxisPosition
	{
		Left,
		Right,
		Center
	};

	enum struct Axis
	{
		X,
		Y
	};

	void Insert(const QuadtreeNode& node, const Rectangle<N>& indexedArea, const R& r, Index depth)
	{
		const auto posX = DetermineAxisPosition(indexedArea, r, Axis::X);
		const auto posY = DetermineAxisPosition(indexedArea, r, Axis::Y);

		if (depth < maxDepth && posX != AxisPosition::Center && posY != AxisPosition::Center)
		{
			const auto quadrant = DetermineContainingQuadrant(indexedArea, r);
			const auto child = GetChildQuadtreeNode(quadrant);
			Insert(quadtreeNodes.Get(child), GetChildArea(quadrant, indexedArea), r, depth + 1);
			return;
		}
	}

	Rectangle<N> GetChildArea(Quadrant quadrant, const Rectangle<N>& indexedArea)
	{

	}

	Index GetChildQuadtreeNode(const QuadtreeNode& node, Quadrant quadrant)
	{
		if (node.children[quadrant] == -1)
		{
			node.children[quadrant] = quadtreeNodes.Insert();
		}
		return node.children[quadrant];
	}

	AxisPosition DetermineAxisPosition(const Rectangle<N>& indexedArea, const R& r, Axis axis) const
	{
		const auto axisCenter = (axis == Axis::X)
			? indexedArea.GetCenterX()
			: indexedArea.GetCenterY();

		const auto rCenter = (axis == Axis::X)
			? r.GetCenterX()
			: r.GetCenterY();

		const auto halfSide = (axis == Axis::X)
			? r.GetHalfWidth()
			: r.GetHalfHeight();

		if (rCenter - halfSide <= axisCenter && axisCenter <= rCenter + halfSide)
		{
			return AxisPosition::Center;
		}
		if (axisCenter < rCenter)
		{
			return AxisPosition::Right;
		}
		return AxisPosition::Left;
	}

	Quadrant DetermineContainingQuadrant(const Rectangle<N>& indexedArea, const R& r)
	{
		if (r.GetCenterX() < indexedArea.GetCenterX())
		{
			return r.GetCenterY() < indexedArea.GetCenterY()
				? Quadrant::SW
				: Quadrant::NW;
		}
		else
		{
			return r.GetCenterY() < indexedArea.GetCenterY()
				? Quadrant::SE
				: Quadrant::NE;
		}
	}

	FreeList<QuadtreeNode> quadtreeNodes;
	FreeList<AxisBinaryTreeNode> axisBinaryTreeNodes;
	FreeList<LinkedListNode> linkedListNodes;
	Rectangle<N> indexedArea;
	Index root;
	Index maxDepth;
};
