#include "Algo2d.h"

bool geodb::algo::LineLineIntersection
(
    double startX1, double startY1, double endX1, double endY1,
    double startX2, double startY2, double endX2, double endY2
)
{
	double q = (startY1 - startY2) * (endX2 - startX2) - (startX1 - startX2) * (endY2 - startY2);
	double d = (endX1 - startX1) * (endY2 - startY2) - (endY1 - startY1) * (endX2 - startX2);

	if (d == 0)
	{
		return false;
	}

	double r = q / d;

	q = (startY1 - startY2) * (endX1 - startX1) - (startX1 - startX2) * (endY1 - startY1);
	double s = q / d;

	if (r < 0 || r > 1 || s < 0 || s > 1)
	{
		return false;
	}

	return true;
}

bool geodb::algo::LineRectangleIntersection
(
	double startX, double startY, double endX, double endY, 
	double rectTopLeftX, double rectTopLeftY, double rectWidth, double rectHeight
)
{
	const auto leftX = rectTopLeftX;
	const auto topY = rectTopLeftY;
	const auto rightX = rectTopLeftX + rectWidth;
	const auto bottomY = rectTopLeftY - rectHeight;

	return geodb::algo::LineLineIntersection(startX, startY, endX, endY, leftX, topY, rightX, topY) ||
		   geodb::algo::LineLineIntersection(startX, startY, endX, endY, rightX, topY, rightX, bottomY) ||
		   geodb::algo::LineLineIntersection(startX, startY, endX, endY, rightX, bottomY, leftX, bottomY) ||
		   geodb::algo::LineLineIntersection(startX, startY, endX, endY, leftX, bottomY, leftX, topY);
}
