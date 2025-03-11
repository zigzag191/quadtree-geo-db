#pragma once

#include "Common.h"

template<Numeric T>
class Rectangle
{
public:
	Rectangle(T topLeftX, T topLeftY, T width, T height)
		: halfWidth{ width / 2 }
		, halfHeight{ height / 2 }
		, centerX{ topLeftX + halfWidth }
		, centerY{ topLeftY - halfHeight }
	{ }

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
