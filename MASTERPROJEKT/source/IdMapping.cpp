#include "IdMapping.h"
#include <stdio.h>
using namespace cv;
std::vector<Point2f> mv;

std::vector<Point2f> IdMapping::CalculateMotionVector(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm) {

	//tm.at(0)->getCenter().y;
	return std::vector<Point2f>();
}

bool IdMapping::MotionEstimationPerMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm)
{
	bool isSameMarker = false;

	for each (Marker* m in tm)
	{
	}

	return isSameMarker;
}

bool IdMapping::isConstantMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm)
{
	bool isConstant = false;
	float thres = 0.01f;

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

bool IdMapping::isMarkerOutOfField() {

	return false;
}

// Proofs if this is moved Marker with known identity
bool IdMapping::isTrackedMarker(std::vector<Point2f> points, unsigned char markedCorner)
{
	//TODO
	return false;
}


IdMapping::IdMapping()
{

}

IdMapping::~IdMapping()
{

}
