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
			: m_halfWidth{ halfWidth }
			, m_halfHeight{ halfHeight }
			, m_centerX{ centerX }
			, m_centerY{ centerY }
			, m_area{ m_halfHeight * m_halfHeight * 4 }
		{}

		// TODO: fixme
		Rectangle() {}

		inline T GetCenterX() const
		{
			return m_centerX;
		}

		inline T GetCenterY() const
		{
			return m_centerY;
		}

		inline T GetHalfWidth() const
		{
			return m_halfWidth;
		}

		inline T GetHalfHeight() const
		{
			return m_halfHeight;
		}

		inline T GetArea() const
		{
			return m_area;
		}

	private:
		T m_halfWidth = 0;
		T m_halfHeight = 0;
		T m_centerX = 0;
		T m_centerY = 0;
		T m_area = 0;
	};
}
