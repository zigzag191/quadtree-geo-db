#include <iostream>
#include <random>

#include "Quadtree.h"
#include "Rectangle.h"

using namespace quadtree;

namespace
{
	class Random
	{
	public:
		Random()
			: random{ std::random_device{}() }
			, distribution{ 0, 1 }
		{ }

		float Next(float lower, float upper)
		{
			return lower + distribution(random) * (upper - lower);
		}

	private:
		std::mt19937 random;
		std::uniform_real_distribution<float> distribution;
	};

	Random RNG;

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

	Rectangle<float> RandomRectangle(const Rectangle<float>& area, float dimentionsDecrease = 1.0f)
	{
		static constexpr auto eps = 0.001f;

		const auto areaMinX = area.GetCenterX() - area.GetHalfWidth();
		const auto areaMaxX = area.GetCenterX() + area.GetHalfWidth();

		const auto areaMinY = area.GetCenterY() - area.GetHalfHeight();
		const auto areaMaxY = area.GetCenterY() + area.GetHalfHeight();

		const auto topLeftX = RNG.Next(areaMinX, areaMaxX - eps);
		const auto topLeftY = RNG.Next(areaMinY, areaMaxY - eps);

		const auto width = RNG.Next(eps, areaMaxX - topLeftX);
		const auto height = RNG.Next(eps, topLeftY - areaMinY);

		return Rectangle{ topLeftX, topLeftY, width / dimentionsDecrease, height / dimentionsDecrease };
	}
}

int main()
{
	const auto indexedArea = Rectangle<float>::Of(-100.0f, 100.0f, 200.0f, 200.0f);
	auto quadtree = Quadtree<float, Rectangle<float>>{ indexedArea, 10 };

	std::cout << "Inserting...\n";

	for (int i = 0; i < 10000000; ++i)
	{
		quadtree.Insert(RandomRectangle(indexedArea));
	}

	for (int i = 0; i < 100; ++i)
	{
		std::cout << "Query " << (i + 1) << "...\n";
		quadtree.Query(RandomRectangle(indexedArea, 2));
	}

	return 0;
}
