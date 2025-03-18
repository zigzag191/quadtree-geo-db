#pragma once

#include <concepts>
#include <array>

#include "Common.h"
#include "Rectangle.h"

template<typename T, typename N>
concept Rectangular = Numeric<N> && requires(T a)
{
	{ a.GetCenterX() } -> std::convertible_to<N>;
	{ a.GetCenterY() } -> std::convertible_to<N>;
	{ a.GetHalfWidth() } -> std::convertible_to<N>;
	{ a.GetHalfHeight() } -> std::convertible_to<N>;
};

template<Numeric N, Rectangular<N> R, template<typename> typename AllocationStrategy>
class Quadtree
{
public:
	Quadtree(const Rectangle<N>& indexedArea, int maxDepth)
		: indexedArea{ indexedArea }
		, maxDepth{ maxDepth }
	{
		root = quadtreeNodesAllocator.New();
	}

	void Insert(const R& r)
	{
		Insert(*root, indexedArea, r, 1);
	}

private:
	enum struct Quadrant
	{
		NE = 0,
		NW = 1,
		SW = 2,
		SE = 3
	};

	enum struct AxisPosition
	{
		Left = 0,
		Right = 1,
		Center = 2
	};

	enum struct Axis
	{
		X,
		Y
	};

	struct LinkedListNode
	{
		R element;
		LinkedListNode* next = nullptr;
	};

	struct AxisBinaryTreeNode
	{
		AxisBinaryTreeNode* left = nullptr;
		AxisBinaryTreeNode* right = nullptr;
		LinkedListNode* firstElement = nullptr;
	};

	struct QuadtreeNode
	{
		std::array<QuadtreeNode*, 4> children = { { nullptr, nullptr, nullptr, nullptr } };
		AxisBinaryTreeNode* xAxis = nullptr;
		AxisBinaryTreeNode* yAxis = nullptr;
	};

	void Insert(QuadtreeNode& node, const Rectangle<N>& indexedArea, const R& r, Index depth)
	{
		const auto posX = DetermineAxisPosition(indexedArea, r, Axis::X);
		const auto posY = DetermineAxisPosition(indexedArea, r, Axis::Y);

		if (depth < maxDepth && posX != AxisPosition::Center && posY != AxisPosition::Center)
		{
			auto quadrant = Quadrant{};
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

			const auto childIndex = static_cast<int>(quadrant);
			if (node.children[childIndex] == nullptr)
			{
				node.children[childIndex] = quadtreeNodesAllocator.New();
			}

			Insert(
				*node.children[childIndex],
				GetChildArea(quadrant, indexedArea), 
				r, 
				depth + 1
			);

			return;
		}

		if (posX == AxisPosition::Center)
		{
			if (node.yAxis == nullptr)
			{
				node.yAxis = axisBinaryTreeNodesAllocator.New();
			}
			InsertIntoAxis(*node.yAxis, indexedArea, r, Axis::Y, 0);
		}
		else
		{
			if (node.xAxis == nullptr)
			{
				node.xAxis = axisBinaryTreeNodesAllocator.New();
			}
			InsertIntoAxis(*node.xAxis, indexedArea, r, Axis::X, 0);
		}
	}

	void InsertIntoAxis
	(
		AxisBinaryTreeNode& node, 
		const Rectangle<N>& indexedArea, 
		const R& r,
		Axis axis,
		Index depth
	)
	{
		const auto pos = DetermineAxisPosition(indexedArea, r, axis);

		if (pos == AxisPosition::Center || depth >= maxDepth)
		{
			auto next = linkedListNodesAllocator.New();
			next->element = r;
			next->next = node.firstElement;
			node.firstElement = next;
		}
		else if (pos == AxisPosition::Left)
		{
			if (node.left == nullptr)
			{
				node.left = axisBinaryTreeNodesAllocator.New();
			}
			InsertIntoAxis(*node.left, GetChildAxisArea(pos, axis, indexedArea), r, axis, depth + 1);
		}
		else
		{
			if (node.right == nullptr)
			{
				node.right = axisBinaryTreeNodesAllocator.New();
			}
			InsertIntoAxis(*node.right, GetChildAxisArea(pos, axis, indexedArea), r, axis, depth + 1);
		}
	}

	Rectangle<N> GetChildArea(Quadrant quadrant, const Rectangle<N>& indexedArea)
	{
		static constexpr std::array<int, 4> directionsX = { { 1, -1, -1,  1 } };
		static constexpr std::array<int, 4> directionsY = { { 1,  1, -1, -1 } };

		const auto index = static_cast<int>(quadrant);

		const auto childWidth = indexedArea.GetHalfWidth() / 2;
		const auto childHeight = indexedArea.GetHalfHeight() / 2;

		return Rectangle{
			indexedArea.GetCenterX() + (childWidth * directionsX[index]),
			indexedArea.GetCenterY() + (childHeight * directionsY[index]),
			childWidth,
			childHeight
		};
	}

	Rectangle<N> GetChildAxisArea(AxisPosition position, Axis axis, const Rectangle<N>& indexedArea)
	{
		static constexpr std::array<int, 3> directions = { { -1, 1, 0 } };

		const auto index = static_cast<int>(position);

		if (axis == Axis::X)
		{
			const auto side = indexedArea.GetHalfWidth() / 2;
			return Rectangle{
				side,
				indexedArea.GetHalfHeight(),
				indexedArea.GetCenterX() + side * directions[index],
				indexedArea.GetCenterY()
			};
		}
		const auto side = indexedArea.GetHalfHeight() / 2;
		return Rectangle{
			indexedArea.GetHalfWidth(),
			side,
			indexedArea.GetCenterX(),
			indexedArea.GetCenterY() + side * directions[index]
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

	AllocationStrategy<QuadtreeNode> quadtreeNodesAllocator;
	AllocationStrategy<AxisBinaryTreeNode> axisBinaryTreeNodesAllocator;
	AllocationStrategy<LinkedListNode> linkedListNodesAllocator;
	Rectangle<N> indexedArea;
	int maxDepth;
	QuadtreeNode* root = nullptr;
};
