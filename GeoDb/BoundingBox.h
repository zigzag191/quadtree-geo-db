#pragma once

#include "../Quadtree/Rectangle.h"

namespace geodb
{
	class BoundingBox : public quadtree::Rectangle<double>
	{
	public:
		BoundingBox(std::size_t objectIndex, double topLeftX, double topLeftY, double width, double height)
			: Rectangle{ topLeftX + width / 2, topLeftY - height / 2, width / 2, height / 2 }
			, m_objectIndex{ objectIndex }
		{
		}

		BoundingBox() {}

		inline std::size_t GetObjectIndex() const { return m_objectIndex; }

	private:
		std::size_t m_objectIndex = 0;
	};
}
