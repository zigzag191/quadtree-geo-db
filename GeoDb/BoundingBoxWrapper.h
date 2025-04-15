#pragma once

#include "Map.h"
#include "../Quadtree/Rectangle.h"

namespace geodb
{
	class BoundngBoxWrapper
	{
	public:
		BoundngBoxWrapper(std::size_t objectIndex, const quadtree::Rectangle<double>* delegate)
			: m_objectIndex{ objectIndex }
			, m_delegate{ delegate }
		{ }

		BoundngBoxWrapper() {}

		inline double GetCenterX() const
		{
			return m_delegate->GetCenterX();
		}

		inline double GetCenterY() const
		{
			return m_delegate->GetCenterY();
		}

		inline double GetHalfWidth() const
		{
			return m_delegate->GetHalfWidth();
		}

		inline double GetHalfHeight() const
		{
			return m_delegate->GetHalfHeight();
		}

		inline std::size_t GetObjectIndex() const { return m_objectIndex; }

	private:
		std::size_t m_objectIndex = 0;
		const quadtree::Rectangle<double>* m_delegate = nullptr;
	};
}
