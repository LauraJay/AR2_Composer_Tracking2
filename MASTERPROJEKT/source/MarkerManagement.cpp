#include "MarkerManagement.h"
using namespace cv;

std::vector<Marker*> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

void MarkerManagement::trackMarker(RotatedRect rect, unsigned char markedCorner,Size size)
{

		std::vector<Point2f> rectPoints = normalizeRectPoints(rect, size);
		Point2f center = rect.center;
		center.x = center.x / size.width;
		center.y = center.y / size.height;
		IdMapping* im = new IdMapping();
		//isConstant Berechnung Laura
		if (!im->isConstantMarker(rectPoints, center, markedCorner, trackedMarker))
		//isConstant Berechnung Vera
		//if (!im->isConstantMarker(rectPoints, markedCorner, trackedMarker))
			if (!im->isTranslatedMarker(rectPoints, center, markedCorner, trackedMarker))
				//if (!im->isRotatedMarker(rectPoints, center, markedCorner, trackedMarker))
			//if (!im->isConstantMarker())
			//if (!im->isTrackedMarker(rectPoints, markedCorner))
			registerNewMarker(rectPoints, center, markedCorner);
	
}

MarkerManagement::MarkerManagement()
{

}


MarkerManagement::~MarkerManagement()
{
}

Point2f normalizeCoord(Point2f p, Size size) {

	p.x = p.x / size.width;
	p.y = p.y/size.height;

	return p;
}

std::vector<Point2f> MarkerManagement::normalizeRectPoints(RotatedRect rect,Size size) {
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

// checks if Marker is removed from Playfield
bool MarkerManagement::isMarkerOutOfField()
{
	//TODO
	int id;
	deleteMarker(id);
	return false;
}

// Delete Function of Marker
void MarkerManagement::deleteMarker(int id)
{
	//Marker* m = trackedMarker[id];
	trackedMarker.erase(trackedMarker.begin() + id);
	//delete &m;
}

// inits a new trackedMarker
void MarkerManagement::registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner)
{
	if (trackedMarker.size() <= 3000) {
		Marker* m = new Marker(trackedMarker.size() + 1, rectPoints, center, markedCorner);
		trackedMarker.push_back(m);
	}
}
