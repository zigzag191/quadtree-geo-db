#pragma once

#include <vector>

struct Node
{
	double longitude;
	double latitude;
};

struct Way
{
	std::vector<std::size_t> nodes;
};

struct Map
{
	std::vector<Node> nodes;
	std::vector<Way> ways;
};
