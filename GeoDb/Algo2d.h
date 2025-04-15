#pragma once

namespace geodb
{
	namespace algo
	{
		bool LineLineIntersection
		(
			double startX1, double startY1, double endX1, double endY1,
			double startX2, double startY2, double endX2, double endY2
		);

		bool LineRectangleIntersection
		(
			double startX, double startY, double endX, double endY,
			double rectTopLeftX, double rectTopLeftY, double rectWidth, double rectHeight
		);
	}
}
