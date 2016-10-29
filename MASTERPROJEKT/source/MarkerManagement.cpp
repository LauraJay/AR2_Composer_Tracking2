#include "MarkerManagement.h"
using namespace cv;

std::array<Marker*,200> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

std::vector<int> MarkerManagement::getTakenIDVec()
{
	return takenIDVec;
}

void MarkerManagement::trackMarker(RotatedRect rect, unsigned char markedCorner, Size size)
{
	IdMapping* im = new IdMapping();
	std::vector<Point2f> rectPoints = normalizeRectPoints(rect, size);
	Point2f center = normalizeCoord(rect.center,size);
	std::vector<Point2f> motionCenterVecs = im->CalculateMotionVectorCenter(rectPoints, center, markedCorner,trackedMarker,takenIDVec);
	std::vector<Point2f> motionMarkCornerVecs = im->CalculateMotionVectorMarkedCorner(rectPoints, center, markedCorner, trackedMarker,takenIDVec);
	int matchID = im->isConstantMarker(rectPoints, center, markedCorner, trackedMarker, motionCenterVecs, motionMarkCornerVecs,takenIDVec);
	if (matchID>0) {
		CurrentMarker(trackedMarker[matchID], rectPoints, center, markedCorner);
	}
	else if (matchID= im->isTranslatedMarker(rectPoints, center, markedCorner, trackedMarker, motionCenterVecs, motionMarkCornerVecs,takenIDVec)>0) {
		CurrentMarker(trackedMarker[matchID], rectPoints, center, markedCorner);
		}
	/*else if (matchID= IdMapping::isRotatedMarker(rectPoints, center, markedCorner, trackedMarker)>0) {
		CurrentMarker(trackedMarker[matchID], rectPoints, center, markedCorner);
	}*/
	else{
		registerNewMarker(rectPoints, center, markedCorner);
	}
	delete im;
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
	Marker* m = trackedMarker[id];
	m->setId(0);
	openIDQueue.push(id);
	for (int i = 0; i < takenIDVec.size(); i++)
	{
		if (takenIDVec[i] == id) {
			takenIDVec.erase(takenIDVec.begin() + i);
			break;
		}
	}
	//delete &m;
}


// inits a new trackedMarker
void MarkerManagement::registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner)
{
	int id = openIDQueue.front();
	openIDQueue.pop();
	Marker* m = new Marker(id, rectPoints, center, markedCorner);
	trackedMarker[id] = m;
	takenIDVec.push_back(id);
}

void MarkerManagement::CurrentMarker(Marker* tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner) {
	tm->setMarkedCornerID(markedCorner);
	tm->setPoints(rectPoints, center);
	tm->setAngle(tm->computeAngle(markedCorner, rectPoints));
}



MarkerManagement::MarkerManagement()
{
	for (size_t i = 1 ; i < 200; i++)
	{
		openIDQueue.push(i);
	}
}


MarkerManagement::~MarkerManagement()
{

}