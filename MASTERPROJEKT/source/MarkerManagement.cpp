#include "MarkerManagement.h"
using namespace cv;


bool MarkerManagement::isConstantMarker(std::vector<Point2f> points, unsigned char markedCorner)
{
	bool isConstant = false;
	int thres = 10;

	for each (Marker* m in trackedMarker)
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


std::vector<Marker*> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

void MarkerManagement::trackMarker(RotatedRect rect, unsigned char markedCorner)
{
	//TODO
	std::vector<Point2f> rectPoints = rectTOVectorPoints(rect);
	Point2f center = rect.center;
	if (!isConstantMarker(rectPoints, markedCorner))
		if (!isTrackedMarker(rectPoints, markedCorner))
			registerNewMarker(rectPoints, center, markedCorner);
}

MarkerManagement::MarkerManagement()
{

}


MarkerManagement::~MarkerManagement()
{
}

std::vector<Point2f> MarkerManagement::rectTOVectorPoints(RotatedRect rect) {
	std::vector<Point2f> v;
	Point2f p[4];
	rect.points(p);
	for (int i = 0; i < 4; i++)
	{
		v.push_back(p[i]);
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
// Proofs if this is moved Marker with known identity
bool MarkerManagement::isTrackedMarker(std::vector<Point2f> points, unsigned char markedCorner)
{
	//TODO
	return false;
}

// inits a new trackedMarker
void MarkerManagement::registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner)
{
	if (trackedMarker.size() <= 255) {
		Marker* m = new Marker(trackedMarker.size() + 1, rectPoints, center, markedCorner);
		trackedMarker.push_back(m);
	}

}
