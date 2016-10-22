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

bool IdMapping::isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm) {
	/*myfile2.open("isConstant.txt", std::ios::out | std::ios::app);
	myfile2 << "Next Frame " << "\n";*/
	bool isConstant = false;
	float thresholdC = 0.005f;
	float thresholdMC = 0.01f;
	int c = 1;

	for each (Marker* m in tm)
	{
		CalculateMotionVectorCenter(points, center, markedCorner, tm, m->getId() - 1);
		CalculateMotionVectorMarkedCorner(points, center, markedCorner, tm, m->getId() - 1);

		if (abs(mvC.x) + abs(mvC.y) >= thresholdC && abs(mvMC.x) + abs(mvMC.y) >= thresholdMC) {
			c++;
			isConstant = false;
			//myfile2 << "\t Schleifendurchlauf " << c << "\n";
		}
		else {
			isConstant = true;
			/*myfile2 << "\t Schleifenabbruch bei " << c << "\n";
			myfile2 << "\t isConstant auf true gesetzt \n";*/
			break;
		}
	}
	//myfile2.close();
	return isConstant;
}

bool IdMapping::isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm) {
	bool isRotated = false;

	return isRotated;
}

bool IdMapping::isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm){
	bool isTranslated = false;

	return isTranslated;
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
