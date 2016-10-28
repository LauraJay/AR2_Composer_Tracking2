#include "MarkerManagement.h"
using namespace cv;

std::vector<Marker*> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

void MarkerManagement::trackMarker(RotatedRect rect, unsigned char markedCorner, Size size)
{
	std::vector<Point2f> rectPoints = normalizeRectPoints(rect, size);
	Point2f center = normalizeCoord(rect.center,size);
	std::vector<Point2f> motionCenterVecs = IdMapping::CalculateMotionVectorCenter(rectPoints, center, markedCorner,trackedMarker);
	std::vector<Point2f> motionMarkCornerVecs = IdMapping::CalculateMotionVectorMarkedCorner(rectPoints, center, markedCorner, trackedMarker);
	int matchID = IdMapping::isConstantMarker(rectPoints, center, markedCorner, trackedMarker, motionCenterVecs, motionMarkCornerVecs);
	if (matchID>0) {
		CurrentMarker(trackedMarker[matchID-1], rectPoints, center, markedCorner);
	}
	else if (matchID= IdMapping::isTranslatedMarker(rectPoints, center, markedCorner, trackedMarker, motionCenterVecs, motionMarkCornerVecs)>0) {
		CurrentMarker(trackedMarker[matchID-1], rectPoints, center, markedCorner);
		}
	/*else if (matchID= IdMapping::isRotatedMarker(rectPoints, center, markedCorner, trackedMarker)>0) {
		CurrentMarker(trackedMarker[matchID-1], rectPoints, center, markedCorner);
	}*/
	else{
		registerNewMarker(rectPoints, center, markedCorner);
	}
}


Point2f MarkerManagement::normalizeCoord(Point2f p, Size size) {

	p.x = p.x / size.width;
	p.y = p.y / size.height;

	return p;
}

std::vector<Point2f> MarkerManagement::normalizeRectPoints(RotatedRect rect, Size size) {
	std::vector<Point2f> v;
	Point2f p[4];
	rect.points(p);
	for (int i = 0; i < 4; i++)
	{
		Point2f pN = normalizeCoord(p[i], size);
		v.push_back(pN);
	}
	return v;
}


// Delete Function of Marker
void MarkerManagement::deleteMarker(int id)
{

	//Marker* m = trackedMarker[id];
	trackedMarker.erase(trackedMarker.begin() + id);
	idQueue.push(id);
	//delete &m;
}


// inits a new trackedMarker
void MarkerManagement::registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner)
{
	int id = idQueue.front();
	idQueue.pop();
	Marker* m = new Marker(id, rectPoints, center, markedCorner);
	trackedMarker.push_back(m);
}

void MarkerManagement::CurrentMarker(Marker* tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner) {
	tm->setMarkedCornerID(markedCorner);
	tm->setPoints(rectPoints, center);
	tm->computeAngle(markedCorner, rectPoints);
}

MarkerManagement::MarkerManagement()
{
	for (size_t i = 1 ; i < 200; i++)
	{
		idQueue.push(i);
	}
}


MarkerManagement::~MarkerManagement()
{

}