#pragma once
#include <queue>
#include <Marker.h>
#include <IdMapping.h>
using namespace cv;
class MarkerManagement
{
private:
	Point2f normalizeCoord(Point2f p, Size size);
	void deleteMarker(int id);
	std::vector<Point2f> normalizeRectPoints(RotatedRect rect, Size size);
	void registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
protected:
	std::queue <int> takenIDQueue;
	std::vector<int> usedIDVec;
	std::vector<Marker*> trackedMarker;

public:
	void CurrentMarker(Marker* tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	std::vector<Marker*> getTrackedMarker();
	void trackMarker(RotatedRect rect, unsigned char markedCorner, Size size);
	MarkerManagement();
	~MarkerManagement();
};

