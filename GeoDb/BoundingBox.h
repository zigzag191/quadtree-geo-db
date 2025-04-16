#pragma once

#include "Map.h"
#include "ObjectType.h"
#include "../Quadtree/Rectangle.h"

namespace geodb
{
	class BoundngBox : public quadtree::Rectangle<double>
	{
	public:
		BoundngBox(std::size_t objectIndex, ObjectType objectType, const quadtree::Rectangle<double>& delegate)
			: quadtree::Rectangle<double>{ delegate.GetCenterX(), delegate.GetCenterY(), delegate.GetHalfWidth(), delegate.GetHalfHeight() }
			, m_objectType{ objectType }
			, m_objectIndex{ objectIndex }
		{ }

		BoundngBox() {}

		ObjectType GetObjectType() const { return m_objectType; }

		std::size_t GetObjectIndex() const { return m_objectIndex; }

	private:
		ObjectType m_objectType = ObjectType::Node;
		std::size_t m_objectIndex = 0;
	};
}
