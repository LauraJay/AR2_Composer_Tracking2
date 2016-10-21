#include "IdMapping.h"
#include <stdio.h>
using namespace cv;
Point2f mv;
float x;
float y;

Point2f IdMapping::CalculateMotionVector(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm, int nr) {

	//x = points.at(nr).x - tm.at(nr)->getCenter().x;
	//y = points.at(nr).y - tm.at(nr)->getCenter().y;
	//mv = points.at(nr) - tm.at(nr);
	x = 1.2f;
	y = 345.32f;
	mv.x = x;
	mv.y = y;
	return mv;
}
void IdMapping::MotionEstimationPerMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm)
//bool IdMapping::MotionEstimationPerMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm)
{
	Point2f tmp;
	for (int i = 0; i < tm.size(); i++) {
		tmp = CalculateMotionVector(points, markedCorner, tm, i);
		//	printf("mv  %f \n " , i , ": ", tmp.at(0) , " , " , tmp.at(1));
		printf("mv  %i \n ", i);
		printf("mvX %t %f \n ", tmp.x);
		printf("mvY %t %f \n ", tmp.y);

	}

	/*bool isSameMarker = false;

	for each (Marker* m in tm)
	{
	}*/

	//return true;
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
