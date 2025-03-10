#pragma once

#include <concepts>

#include "Common.h"
#include "FreeList.h"
#include "Rectangle.h"

template<typename T, typename N>
concept Rectangular = Numeric<N> && requires(T a)
{
	{ a.GetTopLeftX() } -> std::convertible_to<N>;
	{ a.GetTopLeftY() } -> std::convertible_to<N>;
	{ a.GetWidth() } -> std::convertible_to<N>;
	{ a.GetHeight() } -> std::convertible_to<N>;
};

template<Numeric N, Rectangular<N> R>
class Quadtree
{
public:
	Quadtree(const Rectangle<N>& indexedArea)
		: indexedArea{ indexedArea }
	{
		root = quadtreeNodes.Insert();
	}

	void Insert(const R& r)
	{
		root.Insert(indexedArea, r);
	}

private:
	struct QuadtreeNode
	{
		enum struct Quadrant
		{
			SW = 0,
			SE = 1,
			NE = 2,
			NW = 3
		};

		void Insert(const Rectangle<N>& indexedArea, const R& r)
		{

		}

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

	AxisPosition DetermineAxisPosition(const Rectangle<N>& indexedArea, const R& r, Axis axis) const
	{
		const auto axisCenter = (axis == Axis::X)
			? indexedArea.GetTopLeftX() + indexedArea.GetWidth() / 2
			: indexedArea.GetTopLeftY() + indexedArea.GetHeight() / 2;
		if (axis == Axis::X)
		{
			if (axisCenter >= r.GetTopLeftX() && axisCenter <= r.GetTopLeftX() + r.Width())
			{
				return AxisPosition::Center;
			}
			if (axisCenter < r.GetTopLeftX() + r.GetWith() / 2)
			{
				return AxisPosition::Right;
			}
			return AxisPosition::Left;
		}
	}

	FreeList<QuadtreeNode> quadtreeNodes;
	FreeList<AxisBinaryTreeNode> axisBinaryTreeNodes;
	FreeList<LinkedListNode> linkedListNodes;
	Rectangle<N> indexedArea;
	Index root;
};
