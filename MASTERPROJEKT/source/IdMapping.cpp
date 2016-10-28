#include "IdMapping.h"

using namespace cv;

std::vector<Point2f> IdMapping::CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> trackedMarker) {
	std::vector<Point2f> motionVectors;
	for each (Marker* tm in trackedMarker)
	{
		Point2f mvC;
		mvC.x = center.x - tm->getCenter().x;
		mvC.y = center.y - tm->getCenter().y;
		motionVectors.push_back(mvC);
	}
	return motionVectors;
}

std::vector<Point2f> IdMapping::CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> trackedMarker) {
	std::vector<Point2f> motionVectors;
	for each (Marker* tm in trackedMarker)
	{
	Point2f mvMC;
	mvMC.x = points.at(markedCorner).x - tm->getCenter().x;
	mvMC.y = center.y - tm->getCenter().y;
	motionVectors.push_back(mvMC);
	}
	return motionVectors;
}

int IdMapping::isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, std::vector<Point2f> motionCenterVecs , std::vector<Point2f> motionMarkCornerVecs) {
	/*myfile2.open("isConstant.txt", std::ios::out | std::ios::app);
	myfile2 << "Next Frame " << "\n";*/
	bool isConstant = false;
	int  matchID=0;
	float tCenterConstant = 0.03f;
	float tMarkedCornerConstant = 0.05f;

	for (int i = 0; i < tm.size(); i++)
	{
		Marker* m = tm[i];
		Point2f mvC = motionCenterVecs[i];
		Point2f mvMC = motionMarkCornerVecs[i];
		if (abs(mvC.x) + abs(mvC.y) <= tCenterConstant && abs(mvMC.x) + abs(mvMC.y) <= tMarkedCornerConstant) {
			isConstant = true;
			/*myfile2 << "\t Schleifenabbruch bei " << c << "\n";
			myfile2 << "\t isConstant auf true gesetzt \n";*/
		}
		else {
			isConstant = false;
			//myfile2 << "\t Schleifendurchlauf " << c << "\n";
		}
		if (isConstant) {
			isConstant = true;
			matchID = m->getId();
			break;
		}
	}
	//myfile2.close();
	return matchID;
}

int IdMapping::isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, std::vector<Point2f> motionCenterVecs, std::vector<Point2f> motionMarkCornerVecs) {
	bool isTranslated = false;
	int matchID = 0;
	float tTranslation = 0.2f;
	for (int i = 0; i < tm.size(); i++)
	{
		Marker* m = tm[i];
		Point2f mvC = motionCenterVecs[i];
		Point2f mvMC = motionMarkCornerVecs[i];
		if (abs(mvC.x) + abs(mvC.y) <= tTranslation) {
			isTranslated = true;
		}
		else {
			isTranslated = false;
		}
		if (isTranslated) {
			isTranslated = true;
			matchID = m->getId();
			break;
		}
	}
	return matchID;
}

//Noch mal überdenken!
//int IdMapping::isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm) {
//	bool isRotated = false;
//	int matchID = 0;
//	float tRotattion = 0.03f;
//
//	for each (Marker* m in tm)
//	{
//		CalculateMotionVectorCenter(points, center, markedCorner, tm, m->getId() - 1);
//		CalculateMotionVectorMarkedCorner(points, center, markedCorner, tm, m->getId() - 1);
//
//		if (abs(mvC.x) + abs(mvC.y) <= tCenterConstant && abs(mvMC.x) + abs(mvMC.y) <= tRotattion) {
//			isRotated = true;
//		}
//		else {
//			isRotated = false;
//		}
//		if (isRotated) {
//			isRotated = true;
//			matchID = m->getId();
//			break;
//		}
//	}
//	return matchID;
//}


bool IdMapping::isMarkerOutOfField() {
	return false;
}

IdMapping::IdMapping()
{
}






IdMapping::~IdMapping()
{

}
 