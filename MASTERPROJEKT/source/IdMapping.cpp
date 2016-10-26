#include "IdMapping.h"

using namespace cv;

void IdMapping::CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr) {
	mvC.x = center.x - tm.at(nr)->getCenter().x;
	mvC.y = center.y - tm.at(nr)->getCenter().y;
}

void IdMapping::CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr) {
	mvMC.x = points.at(markedCorner).x - tm.at(nr)->getCenter().x;
	mvMC.y = center.y - tm.at(nr)->getCenter().y;
}

int IdMapping::isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm) {
	/*myfile2.open("isConstant.txt", std::ios::out | std::ios::app);
	myfile2 << "Next Frame " << "\n";*/
	bool isConstant = false;
	int c = 1, matchID=0;

	for each (Marker* m in tm)
	{
		CalculateMotionVectorCenter(points, center, markedCorner, tm, m->getId() - 1 );
		CalculateMotionVectorMarkedCorner(points, center, markedCorner, tm, m->getId() -1);

		if (abs(mvC.x) + abs(mvC.y) <= tCenterConstant && abs(mvMC.x) + abs(mvMC.y) <= tMarkedCornerConstant) {
			isConstant = true;
			/*myfile2 << "\t Schleifenabbruch bei " << c << "\n";
			myfile2 << "\t isConstant auf true gesetzt \n";*/
		}
		else {
			c++;
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

int IdMapping::isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm) {
	bool isTranslated = false;
	int matchID = 0;
	for each (Marker* m in tm)
	{
		CalculateMotionVectorCenter(points, center, markedCorner, tm, m->getId() - 1);
		CalculateMotionVectorMarkedCorner(points, center, markedCorner, tm, m->getId() - 1);

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
int IdMapping::isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm) {
	bool isRotated = false;
	int matchID = 0;
	for each (Marker* m in tm)
	{
		CalculateMotionVectorCenter(points, center, markedCorner, tm, m->getId() - 1);
		CalculateMotionVectorMarkedCorner(points, center, markedCorner, tm, m->getId() - 1);

		if (abs(mvC.x) + abs(mvC.y) <= tCenterConstant && abs(mvMC.x) + abs(mvMC.y) <= tRotattion) {
			isRotated = true;
		}
		else {
			isRotated = false;
		}
		if (isRotated) {
			isRotated = true;
			matchID = m->getId();
			break;
		}
	}
	return matchID;
}


bool IdMapping::isMarkerOutOfField() {

	return false;
}




IdMapping::IdMapping()
{

}

IdMapping::~IdMapping()
{

}
 