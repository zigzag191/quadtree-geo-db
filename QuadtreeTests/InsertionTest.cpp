#include <gtest/gtest.h>

#include "../Quadtree/Quadtree.h"
#include "../Quadtree/Rectangle.h"

using namespace quadtree;

TEST(QuadtreeTest, Insertion) {
	const auto indexedArea = Rectangle<float>::Of(-100.0f, 100.0f, 200.0f, 200.0f);
	auto quadtree = Quadtree<float, Rectangle<float>>{ indexedArea, 10 };

	const auto rectangle = Rectangle<float>::Of(10.0f, 10.0f, 1.0f, 1.0f);
	quadtree.Insert(rectangle);

	const auto searchResults = quadtree.Query(
		Rectangle<float>::Of(10.01f, 10.01f, 1.01f, 1.01f)
	);

	EXPECT_EQ(searchResults.size(), 1);
	EXPECT_EQ(*searchResults[0], rectangle);
}
