#include "IdMapping.h"
#include <stdio.h>
using namespace cv;


bool IdMapping::isConstantMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm)
{
	bool isConstant = false;
	int thres = 10;

	for each (Marker* m in tm)
	{
		if (markedCorner == m->getMarkedCornerID()) {
			std::vector<Point2f> trakedPs = m->getPoints();
			bool isConstMarker = true;
			for each (Point2f trackedP in trakedPs)
			{
				bool isMatch = false;
				for each (Point2f p in points)
				{
					if (trackedP.x + thres >= p.x && trackedP.x - thres <= p.x
						&&trackedP.y + thres >= p.y && trackedP.y - thres <= p.y) {
						isMatch = true;
						break;
					}
				}
				isConstMarker &= isMatch;
				if (!isConstMarker) break;
			}
			if (isConstMarker) {
				isConstant = true;
				break;
			}
		}
	}
	return isConstant;
}

IdMapping::IdMapping()
{

}

IdMapping::~IdMapping()
{

}
