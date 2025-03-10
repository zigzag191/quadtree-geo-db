#pragma once

#include "Common.h"

template<Numeric T>
class Rectangle
{
public:
	Rectangle(T topLeftX, T topLeftY, T width, T height)
		: topLeftX{ topLeftX }
		, topLeftY{ topLeftY }
		, width{ width }
		, height{ height }
	{ }

	T GetTopLeftX() const
	{
		return topLeftX;
	}

	T GetTopLeftY() const
	{
		return topLeftY;
	}

	T GetWidth() const
	{
		return width;
	}

	T GetHeight() const
	{
		return height;
	}

private:
	T topLeftX;
	T topLeftY;
	T width;
	T height;
};
