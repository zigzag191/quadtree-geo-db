#include <iostream>

#include "Quadtree.h"
#include "Rectangle.h"

int main()
{
	auto quadtree = Quadtree<float, Rectangle<float>>{ Rectangle<float>::Of(-10.0f, 10.0f, 20.0f, 20.0f), 10 };

	const auto rectangle = Rectangle<float>::Of(5.0f, 5.0f, 1.0f, 1.0f);

	quadtree.Insert(rectangle);
	return 0;
}
