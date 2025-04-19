#pragma once

#include "Common.h"

namespace quadtree
{
	template<Numeric T>
	class Rectangle
	{
	public:
		static Rectangle Of(T topLeftX, T topLeftY, T width, T height)
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

		T GetCenterX() const
		{
			return m_centerX;
		}

		T GetCenterY() const
		{
			return m_centerY;
		}

		T GetHalfWidth() const
		{
			return m_halfWidth;
		}

		T GetHalfHeight() const
		{
			return m_halfHeight;
		}

		T GetArea() const
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
