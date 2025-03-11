#include <iostream>

#include "Quadtree.h"
#include "Rectangle.h"

int main()
{
	auto quadtree = Quadtree<float, Rectangle<float>>{ Rectangle{ -10.0f, 10.0f, 20.0f, 20.0f }, 10 };

	return 0;
}
