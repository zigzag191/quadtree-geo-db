#include <gtest/gtest.h>

#include "../Quadtree/Quadtree.h"
#include "../Quadtree/Rectangle.h"

using namespace quadtree;

TEST(QuadtreeTest, BasicInsertion)
{
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

TEST(QuadtreeTest, DeeplyNestedElement)
{
    const auto area = quadtree::Rectangle<float>::Of(0.0f, 16.0f, 16.0f, 16.0f);
    auto quadtree = quadtree::Quadtree<float, Rectangle<float>>(area, 4);

    const auto rectangle = quadtree::Rectangle<float>::Of(4.5f, 6.5f, 0.1f, 0.1f);
    quadtree.Insert(rectangle);

    const auto searchArea = quadtree::Rectangle<float>::Of(3.0f, 7.0f, 2.0f, 2.0f);
    auto results = quadtree.Query(searchArea);

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]->GetCenterX(), 4.55f);
    EXPECT_EQ(results[0]->GetCenterY(), 6.45f);
}

TEST(QuadtreeTest, LargeElementSpanningMultipleQuadrants)
{
    const auto area = quadtree::Rectangle<float>::Of(0.0f, 100.0f, 100.0f, 100.0f);
    auto quadtree = quadtree::Quadtree<float, Rectangle<float>>(area, 4);

    const auto rect = quadtree::Rectangle<float>::Of(10.0f, 90.0f, 80.0f, 80.0f);
    quadtree.Insert(rect);

    const auto searchNE = quadtree::Rectangle<float>::Of(65.0f, 85.0f, 20.0f, 20.0f);
    const auto searchNW = quadtree::Rectangle<float>::Of(15.0f, 85.0f, 20.0f, 20.0f);
    const auto searchSW = quadtree::Rectangle<float>::Of(15.0f, 35.0f, 20.0f, 20.0f);
    const auto searchSE = quadtree::Rectangle<float>::Of(65.0f, 35.0f, 20.0f, 20.0f);

    EXPECT_EQ(quadtree.Query(searchNE).size(), 1);
    EXPECT_EQ(quadtree.Query(searchNW).size(), 1);
    EXPECT_EQ(quadtree.Query(searchSW).size(), 1);
    EXPECT_EQ(quadtree.Query(searchSE).size(), 1);
}

TEST(QuadtreeTest, ElementsOnBoundary)
{
    const auto area = quadtree::Rectangle<float>::Of(0.0f, 100.0f, 100.0f, 100.0f);
    auto quadtree = quadtree::Quadtree<float, Rectangle<float>>(area, 4);

    Rectangle<float> rect{ 50.0f, 50.0f, 5.0f, 5.0f };
    quadtree.Insert(rect);

    Rectangle<float> searchArea{ 50.0f, 50.0f, 5.0f, 5.0f };
    auto results = quadtree.Query(searchArea);

    ASSERT_EQ(results.size(), 1);
}

TEST(QuadtreeTest, InsertAndQueryMultipleElements)
{
    const auto area = quadtree::Rectangle<float>::Of(0.0f, 100.0f, 100.0f, 100.0f);
    auto quadtree = quadtree::Quadtree<float, Rectangle<float>>(area, 4);

    Rectangle<float> rect1{ 10.0f, 10.0f, 5.0f, 5.0f };
    Rectangle<float> rect2{ 20.0f, 20.0f, 5.0f, 5.0f };
    Rectangle<float> rect3{ 80.0f, 80.0f, 5.0f, 5.0f };

    quadtree.Insert(rect1);
    quadtree.Insert(rect2);
    quadtree.Insert(rect3);

    Rectangle<float> searchArea{ 15.0f, 15.0f, 15.0f, 15.0f };
    auto results = quadtree.Query(searchArea);

    ASSERT_EQ(results.size(), 2);

    Rectangle<float> searchAll{ 50.0f, 50.0f, 50.0f, 50.0f };
    results = quadtree.Query(searchAll);
    EXPECT_EQ(results.size(), 3);
}

TEST(QuadtreeTest, IntersectingElementsAreFoundCorrectly)
{
    const auto area = quadtree::Rectangle<float>::Of(0.0f, 100.0f, 100.0f, 100.0f);
    auto quadtree = quadtree::Quadtree<float, Rectangle<float>>(area, 4);

    Rectangle<float> rect1{ 30.0f, 30.0f, 10.0f, 10.0f };
    Rectangle<float> rect2{ 35.0f, 35.0f, 10.0f, 10.0f };
    Rectangle<float> rect3{ 50.0f, 50.0f, 5.0f, 5.0f };
    Rectangle<float> rect4{ 60.0f, 60.0f, 15.0f, 15.0f };

    quadtree.Insert(rect1);
    quadtree.Insert(rect2);
    quadtree.Insert(rect3);
    quadtree.Insert(rect4);

    {
        Rectangle<float> searchAll{ 50.0f, 50.0f, 50.0f, 50.0f };
        auto results = quadtree.Query(searchAll);
        EXPECT_EQ(results.size(), 4);
    }

    {
        Rectangle<float> searchArea{ 25.0f, 25.0f, 10.0f, 10.0f };
        auto results = quadtree.Query(searchArea);
        ASSERT_EQ(results.size(), 2);

        std::set<std::pair<float, float>> found;
        for (const auto* r : results)
        {
            found.insert({ r->GetCenterX(), r->GetCenterY() });
        }

        EXPECT_TRUE(found.count({ 30.0f, 30.0f }));
        EXPECT_TRUE(found.count({ 35.0f, 35.0f }));
        EXPECT_FALSE(found.count({ 50.0f, 50.0f }));
        EXPECT_FALSE(found.count({ 60.0f, 60.0f }));
    }

    {
        Rectangle<float> searchArea{ 40.0f, 40.0f, 15.0f, 15.0f };
        auto results = quadtree.Query(searchArea);
        ASSERT_EQ(results.size(), 4);

        std::set<std::pair<float, float>> found;
        for (const auto* r : results)
        {
            found.insert({ r->GetCenterX(), r->GetCenterY() });
        }

        EXPECT_TRUE(found.count({ 30.0f, 30.0f }));
        EXPECT_TRUE(found.count({ 35.0f, 35.0f }));
        EXPECT_TRUE(found.count({ 50.0f, 50.0f }));
        EXPECT_TRUE(found.count({ 60.0f, 60.0f }));
    }

    {
        Rectangle<float> pointQuery{ 45.0f, 45.0f, 0.1f, 0.1f };
        auto results = quadtree.Query(pointQuery);
        ASSERT_EQ(results.size(), 3);

        std::set<std::pair<float, float>> found;
        for (const auto* r : results)
        {
            found.insert({ r->GetCenterX(), r->GetCenterY() });
        }

        EXPECT_TRUE(found.count({ 35.0f, 35.0f }));
        EXPECT_TRUE(found.count({ 50.0f, 50.0f }));
        EXPECT_TRUE(found.count({ 60.0f, 60.0f }));
    }
}
