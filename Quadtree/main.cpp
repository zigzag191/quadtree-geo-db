#include <iostream>

#include "Quadtree.h"
#include "Rectangle.h"

namespace
{
	template<typename N>
	void PrintRectangle(const Rectangle<N>& r)
	{
		std::cout
			<< "Rectangle[centerX: " << r.GetCenterX()
			<< ", centerY: " << r.GetCenterY()
			<< ", width: " << r.GetHalfWidth() * 2
			<< ", height: " << r.GetHalfHeight() * 2
			<< "]\n";
	}
}

int main()
{
	const auto indexedArea = Rectangle<float>::Of(-10.0f, 10.0f, 20.0f, 20.0f);
	auto quadtree = Quadtree<float, Rectangle<float>>{ indexedArea, 10 };

	quadtree.Insert(Rectangle<float>::Of(5.0f, 5.0f, 1.0f, 1.0f));
	quadtree.Insert(Rectangle<float>::Of(-5.0f, -5.0f, 2.0f, 2.0f));

	for (const auto rectangle : quadtree.Query(indexedArea))
	{
		PrintRectangle(*rectangle);
	}

	return 0;
}
