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
			const auto halfWidth = width / static_cast<T>(2);
			const auto halfHeight = height / static_cast<T>(2);
			return Rectangle{ topLeftX + halfWidth, topLeftY - halfHeight, halfWidth, halfHeight };
		}

		Rectangle(T centerX, T centerY, T halfWidth, T halfHeight)
			: m_halfWidth{ halfWidth }
			, m_halfHeight{ halfHeight }
			, m_centerX{ centerX }
			, m_centerY{ centerY }
		{}

		friend bool operator==(const Rectangle<T>& r1, const Rectangle<T>& r2)
		{
			return r1.m_halfWidth == r2.m_halfWidth &&
				   r1.m_halfHeight == r2.m_halfHeight &&
				   r1.m_centerX == r2.m_centerX &&
				   r1.m_centerY == r2.m_centerY;
		}

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
			return m_halfWidth * m_halfHeight * static_cast<T>(4);
		}

	private:
		T m_halfWidth = static_cast<T>(0);
		T m_halfHeight = static_cast<T>(0);
		T m_centerX = static_cast<T>(0);
		T m_centerY = static_cast<T>(0);
	};
}
