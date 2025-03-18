#pragma once

#include <vector>
#include <variant>

#include "Common.h"

template<typename T>
class FreeList
{
private:
	using Index = std::int32_t;

public:
	FreeList()
		: firstFree{ -1 }
	{
	}

	inline T* New()
	{
		if (firstFree != -1)
		{
			const auto index = firstFree;
			firstFree = std::get<Index>(data[firstFree]);
			std::get<T>(data[index]) = T{};
			return &std::get<T>(data[index]);
		}
		data.push_back(T{});
		return &std::get<T>(data[data.size() - 1]);
	}

	inline void Delete(T* p)
	{
		const auto index = std::distance(&data[0], p);
		data[index] = firstFree;
		firstFree = index;
	}

private:
	std::vector<std::variant<T, Index>> data;
	Index firstFree;
};
