#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <string_view>
#include <stdexcept>

#include "../Quadtree/Rectangle.h"

namespace geodb
{
	class Node
	{
	public:
		Node(double x, double y)
			: m_x{ x }
			, m_y{ y }
		{ }

		double GetX() const { return m_x; }

		double GetY() const { return m_y; }

	private:
		double m_x;
		double m_y;
	};

	class Way
	{
	public:
		Way(std::vector<std::size_t> nodes, const quadtree::Rectangle<double>& boundingBox)
			: m_boundingBox{ boundingBox }
			, m_nodes{ std::move(nodes) }
		{ }

		const std::vector<std::size_t>& GetNodes() const { return m_nodes; }

		const quadtree::Rectangle<double>& GetBoundingBox() const { return m_boundingBox; };

	private:
		quadtree::Rectangle<double> m_boundingBox;
		std::vector<std::size_t> m_nodes;
	};

	class Tag
	{
	public:
		Tag(std::string key, std::string value)
			: m_key{ std::move(key) }
			, m_value{ std::move(value) }
		{ }

		std::string_view GetKey() const { return m_key; }

		std::string_view GetValue() const { return m_value; }

	private:
		std::string m_key;
		std::string m_value;
	};

	class Map
	{
	public:
		std::vector<Node>& GetNodes() { return m_nodes; }

		std::vector<Way>& GetWays() { return m_ways; }

		const std::vector<Node>& GetNodes() const { return m_nodes; }

		const std::vector<Way>& GetWays() const { return m_ways; }

		const std::vector<Tag>* GetObjectTags(std::size_t objectId) const
		{
			return const_cast<Map*>(this)->DoGetObjectTags(objectId);
		}

		std::vector<Tag>* GetObjectTags(std::size_t objectId)
		{
			return DoGetObjectTags(objectId);
		}

		void AddTagToObject(std::size_t objectId, std::string_view key, std::string_view value)
		{
			m_tags[objectId].push_back(Tag{ std::string{ key }, std::string{ value } });
		}

	private:
		std::vector<Tag>* DoGetObjectTags(std::size_t objectId)
		{
			const auto it = m_tags.find(objectId);
			return it == m_tags.end() ? nullptr : &it->second;
		}

	private:
		std::vector<Node> m_nodes;
		std::vector<Way> m_ways;
		std::unordered_map<std::size_t, std::vector<Tag>> m_tags;
	};
}
