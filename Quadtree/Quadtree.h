#pragma once

#include <concepts>
#include <array>

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
		Insert(root, indexedArea, r, 1);
	}

private:
	enum struct Quadrant
	{
		SE = 0,
		SW = 1,
		NW = 2,
		NE = 3
	};

	struct QuadtreeNode
	{
		std::array<Index, 4> children = { { -1, -1, -1, -1 } };
		Index xAxis = -1;
		Index yAxis = -1;
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
			const auto quadrant = Quadrant{};
			if (r.GetCenterX() < indexedArea.GetCenterX())
			{
				quadrant = r.GetCenterY() < indexedArea.GetCenterY()
					? Quadrant::SW
					: Quadrant::NW;
			}
			else
			{
				quadrant = r.GetCenterY() < indexedArea.GetCenterY()
					? Quadrant::SE
					: Quadrant::NE;
			}

			if (node.children[quadrant] == -1)
			{
				node.children[quadrant] = quadtreeNodes.Insert();
			}

			Insert(
				quadtreeNodes.Get(node.children[quadrant]), 
				GetChildArea(quadrant, indexedArea), 
				r, 
				depth + 1
			);

			return;
		}

		if (posX == AxisPosition::Center)
		{
			if (node.yAxis == -1)
			{
				node.yAxis = axisBinaryTreeNodes.Insert();
			}
			InsertIntoAxis(axisBinaryTreeNodes.Get(node.yAxis), indexedArea, r, Axis::Y, 0);
		}
		else
		{
			if (node.xAxis == -1)
			{
				node.xAxis = axisBinaryTreeNodes.Insert();
			}
			InsertIntoAxis(axisBinaryTreeNodes.Get(node.xAxis), indexedArea, r, Axis::X, 0);
		}
	}

	void InsertIntoAxis
	(
		const AxisBinaryTreeNode& node, 
		const Rectangle<N>& indexedArea, 
		const R& r,
		Axis axis,
		Index depth
	)
	{
		const auto pos = DetermineAxisPosition(indexedArea, r, axis);

		if (pos == AxisPosition::Center || depth >= maxDepth)
		{
			const auto next = linkedListNodes.Insert();
			const auto listNode = linkedListNodes.Get(next);
			listNode.element = r;
			listNode.next = node.firstElement;
			node.firstElement = next;
			return;
		}
		else if (pos == AxisPosition::Left)
		{
			if (node.left == -1)
			{
				node.left = axisBinaryTreeNodes.Insert();
			}
			//InsertIntoAxis(axisBinaryTreeNodes.Get(node.left), )
		}
		else
		{
			if (node.right == -1)
			{
				node.right = axisBinaryTreeNodes.Insert();
			}
			//InsertIntoAxis(axisBinaryTreeNodes.Get(node.left), )
		}
	}

	Rectangle<N> GetChildArea(Quadrant quadrant, const Rectangle<N>& indexedArea)
	{
		static constexpr std::array<int, 4> directionsX = { { 1, -1, -1,  1 } };
		static constexpr std::array<int, 4> directionsY = { { 1,  1, -1, -1 } };

		const auto childWidth = indexedArea.GetHalfWidth() / 2;
		const auto childHeight = indexedArea.GetHalfHeight() / 2;

		return Rectangle{
			indexedArea.GetCenterX() + (childWidth * directionsX[quadrant]),
			indexedArea.GetCenterY() + (childHeight * directionsY[quadrant]),
			childWidth,
			childHeight
		};
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

	FreeList<QuadtreeNode> quadtreeNodes;
	FreeList<AxisBinaryTreeNode> axisBinaryTreeNodes;
	FreeList<LinkedListNode> linkedListNodes;
	Rectangle<N> indexedArea;
	Index root;
	Index maxDepth;
};
