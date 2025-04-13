#pragma once

#include "Common.h"

namespace quadtree
{
	template<Numeric T>
	class Rectangle
	{
	public:
		inline static Rectangle Of(T topLeftX, T topLeftY, T width, T height)
		{
			const auto halfWidth = width / 2;
			const auto halfHeight = height / 2;
			return Rectangle{ topLeftX + halfWidth, topLeftY - halfHeight, halfWidth, halfHeight };
		}

		Rectangle(T centerX, T centerY, T halfWidth, T halfHeight)
			: halfWidth{ halfWidth }
			, halfHeight{ halfHeight }
			, centerX{ centerX }
			, centerY{ centerY }
		{}

		// TODO: fixme
		Rectangle() {}

		inline T GetCenterX() const
		{
			return centerX;
		}

		inline T GetCenterY() const
		{
			return centerY;
		}

		inline T GetHalfWidth() const
		{
			return halfWidth;
		}

		inline T GetHalfHeight() const
		{
			return halfHeight;
		}

	private:
		T halfWidth = 0;
		T halfHeight = 0;
		T centerX = 0;
		T centerY = 0;
	};
}
