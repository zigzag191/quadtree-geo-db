#pragma once

#include "Common.h"

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
		: halfWidth{ halfWidth }
		, halfHeight{ halfHeight }
		, centerX{ centerX }
		, centerY{ centerY }
	{ }

	// TODO: fixme
	Rectangle() { }

	T GetCenterX() const
	{
		return centerX;
	}

	T GetCenterY() const
	{
		return centerY;
	}

	T GetHalfWidth() const
	{
		return halfWidth;
	}

	T GetHalfHeight() const
	{
		return halfHeight;
	}

private:
	T halfWidth;
	T halfHeight;
	T centerX;
	T centerY;
};
