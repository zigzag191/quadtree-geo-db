#pragma once

#include <vector>
#include <variant>

#include "Common.h"

template<typename T>
class FreeList
{
public:
	FreeList()
		: firstFree{ -1 }
	{
	}

	inline Index Insert()
	{
		if (firstFree != -1)
		{
			auto index = firstFree;
			firstFree = std::get<Index>(data[firstFree]);
			std::get<T>(data[index]) = T{};
			return index;
		}
		data.push_back(T{});
		return data.size() - 1;
	}

	inline void Erase(Index index)
	{
		data[index] = firstFree;
		firstFree = index;
	}

	inline const T& Get(Index index) const
	{
		return data[index];
	}

	inline T& Get(Index index)
	{
		return data[index];
	}

private:
	std::vector<std::variant<T, Index>> data;
	Index firstFree;
};
