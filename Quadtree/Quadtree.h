#pragma once

#include <concepts>
#include <array>
#include <vector>

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

template<Numeric N, Rectangular<N> R>
class Quadtree final
{
public:
	Quadtree(const Quadtree& other) = delete;

	Quadtree& operator=(const Quadtree& other) = delete;

	Quadtree(const Rectangle<N>& indexedArea, int maxDepth)
		: indexedArea{ indexedArea }
		, maxDepth{ maxDepth }
	{
		root = new QuadtreeNode;
	}

	void Insert(const R& r)
	{
		Insert(*root, indexedArea, r, 1);
	}

	template<Rectangular<N> Window>
	std::vector<R*> Query(const Window& searchWindow)
	{
		auto result = std::vector<R*>{};
		Query(result, root, indexedArea, searchWindow);
		return result;
	}

	~Quadtree()
	{
		DeleteQuadtree(root);
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
		LinkedListNode* elements = nullptr;
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
				node.children[childIndex] = new QuadtreeNode;
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
				node.yAxis = new AxisBinaryTreeNode;
			}
			InsertIntoAxis(*node.yAxis, indexedArea, r, Axis::Y, 0);
		}
		else
		{
			if (node.xAxis == nullptr)
			{
				node.xAxis = new AxisBinaryTreeNode;
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
			auto next = new LinkedListNode;
			next->element = r;
			next->next = node.elements;
			node.elements = next;
		}
		else if (pos == AxisPosition::Left)
		{
			if (node.left == nullptr)
			{
				node.left = new AxisBinaryTreeNode;
			}
			InsertIntoAxis(*node.left, GetChildAxisArea(pos, axis, indexedArea), r, axis, depth + 1);
		}
		else
		{
			if (node.right == nullptr)
			{
				node.right = new AxisBinaryTreeNode;
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

	template<Rectangular<N> Window>
	void Query
	(
		std::vector<R*>& result, 
		QuadtreeNode* node, 
		const Rectangle<N>& indexedArea, 
		const Window& searchWindow
	)
	{
		if (node == nullptr)
		{
			return;
		}
		if (RectanglesIntersect(indexedArea, searchWindow))
		{
			QueryAxisBinaryTree(result, node->xAxis, indexedArea, searchWindow, Axis::X);
			QueryAxisBinaryTree(result, node->yAxis, indexedArea, searchWindow, Axis::Y);
			for (int i = 0; i < 4; ++i)
			{
				const auto quadrant = static_cast<Quadrant>(i);
				Query(result, node->children[i], GetChildArea(quadrant, indexedArea), searchWindow);
			}
		}
	}

	template<Rectangular<N> Window>
	void QueryAxisBinaryTree
	(
		std::vector<R*>& result,
		AxisBinaryTreeNode* node,
		const Rectangle<N>& indexedArea,
		const Window& searchWindow,
		Axis axis
	)
	{
		if (node == nullptr)
		{
			return;
		}
		QueryLinkedList(result, node->elements, searchWindow);
		const auto windowPos = DetermineAxisPosition(indexedArea, searchWindow, axis);
		const auto searchChild = [&](AxisBinaryTreeNode* child, AxisPosition pos)
			{
				if (child != nullptr)
				{
					QueryAxisBinaryTree(
						result, 
						child,
						GetChildAxisArea(pos, axis, indexedArea), 
						searchWindow, 
						axis
					);
				}
			};
		if (windowPos == AxisPosition::Center)
		{
			searchChild(node->left, AxisPosition::Left);
			searchChild(node->right, AxisPosition::Right);
		}
		else if (windowPos == AxisPosition::Left)
		{
			searchChild(node->left, AxisPosition::Left);
		}
		else
		{
			searchChild(node->right, AxisPosition::Right);
		}
	}

	template<Rectangular<N> R1>
	AxisPosition DetermineAxisPosition(const Rectangle<N>& indexedArea, const R1& r, Axis axis) const
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

	template<Rectangular<N> Window>
	void QueryLinkedList(std::vector<R*>& result, LinkedListNode* root, const Window& searchWindow)
	{
		auto current = root;
		while (current != nullptr)
		{
			if (RectanglesIntersect(current->element, searchWindow))
			{
				result.push_back(&current->element);
			}
			current = current->next;
		}
	}

	template<Rectangular<N> R1, Rectangular<N> R2>
	bool RectanglesIntersect(const R1& r1, const R2& r2)
	{
		return (r1.GetCenterY() - r1.GetHalfHeight() <= r2.GetCenterY() + r2.GetHalfHeight())
			&& (r1.GetCenterY() + r1.GetHalfHeight() >= r2.GetCenterY() - r2.GetHalfHeight())
			&& (r1.GetCenterX() - r1.GetHalfWidth() <= r2.GetCenterX() + r2.GetHalfWidth())
			&& (r1.GetCenterX() + r1.GetHalfWidth() >= r2.GetCenterX() - r2.GetHalfWidth());
	}

	void DeleteQuadtree(QuadtreeNode* root)
	{
		if (root != nullptr)
		{
			for (auto child : root->children)
			{
				DeleteQuadtree(child);
			}
			DeleteAxisBinaryTree(root->xAxis);
			DeleteAxisBinaryTree(root->yAxis);
			delete root;
		}
	}

	void DeleteAxisBinaryTree(AxisBinaryTreeNode* root)
	{
		if (root != nullptr)
		{
			DeleteAxisBinaryTree(root->left);
			DeleteAxisBinaryTree(root->right);
			DeleteLinkedList(root->elements);
			delete root;
		}
	}

	void DeleteLinkedList(LinkedListNode* root)
	{
		auto current = root;
		while (current != nullptr)
		{
			auto tmp = current->next;
			delete current;
			current = tmp;
		}
	}

	Rectangle<N> indexedArea;
	int maxDepth;
	QuadtreeNode* root = nullptr;
};
