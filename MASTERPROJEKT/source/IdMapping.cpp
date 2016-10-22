#include "IdMapping.h"
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
using namespace cv;

Point2f mvC;
Point2f mvMC;
float x;
float y;
std::ofstream myfile2;

void IdMapping::CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr) {
	x = center.x - tm.at(nr)->getCenter().x;
	y = center.y - tm.at(nr)->getCenter().y;
	mvC.x = x;
	mvC.y = y;
}

void IdMapping::CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr) {
	x = points.at(markedCorner).x - tm.at(nr)->getCenter().x;
	y = center.y - tm.at(nr)->getCenter().y;
	mvMC.x = x;
	mvMC.y = y;
}

void IdMapping::MotionEstimationPerMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm){

myfile2.open("mv.txt", std::ios::out | std::ios::app);
myfile2 << "Next Frame "  << "\n";
	int c = 1;

	for (int i = 0; i < c; i++) {
		if (c < tm.size()) {
			CalculateMotionVectorCenter(points, center, markedCorner, tm, i);
			myfile2 << "\t Center mvX " << mvC.x << "\n";
			myfile2 << "\t Center mvY " << mvC.y << "\n";
			CalculateMotionVectorMarkedCorner(points, center, markedCorner, tm, i);
			myfile2 << "\t Marked Corner mvX " << mvMC.x << "\n";
			myfile2 << "\t Marked Corner mvY " << mvMC.y << "\n";

			if (isConstantMarker()) {
				myfile2 << "\t Schleifenabbruch bei " << c << "\n";	
			}
			else {
				myfile2 << "\t Schleifendurchlauf " << c << "\n";
				c++;
			}
		}
	}
	myfile2.close();
}

bool IdMapping::isConstantMarker() {
	bool isConstant = false;
	float thresholdC = 0.1f;
	float thresholdMC = 0.1f;
	
	if (abs(mvC.x) + abs(mvC.y) <= thresholdC && abs(mvMC.x) + abs(mvMC.y) <= thresholdMC) {
			isConstant = true;
	}


	return isConstant;
}



//bool IdMapping::isConstantMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm)
//{
//	bool isConstant = false;
//	float thres = 0.01f;
//
//	for each (Marker* m in tm)
//	{
//		if (markedCorner == m->getMarkedCornerID()) {
//			std::vector<Point2f> trakedPs = m->getPoints();
//			bool isConstMarker = true;
//			for each (Point2f trackedP in trakedPs)
//			{
//				bool isMatch = false;
//				for each (Point2f p in points)
//				{
//					if (trackedP.x + thres >= p.x && trackedP.x - thres <= p.x
//						&&trackedP.y + thres >= p.y && trackedP.y - thres <= p.y) {
//						isMatch = true;
//						break;
//					}
//				}
//				isConstMarker &= isMatch;
//				if (!isConstMarker) break;
//			}
//			if (isConstMarker) {
//				isConstant = true;
//				break;
//			}
//		}
//	}
//	return isConstant;
//}

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
