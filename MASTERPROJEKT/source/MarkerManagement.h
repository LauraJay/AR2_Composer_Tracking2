#pragma once
#include <Marker.h>
#include <IdMapping.h>
using namespace cv;
class MarkerManagement
{
private:
	std::vector<Marker*> trackedMarker;
	Point2f normalizeCoord(Point2f p, Size size);
	void deleteMarker(int id);
	std::vector<Point2f> normalizeRectPoints(RotatedRect rect, Size size);
	void registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);

public:
	void CurrentMarker(Marker* tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	std::vector<Marker*> getTrackedMarker();
	void trackMarker(RotatedRect rect, unsigned char markedCorner, Size size);
	MarkerManagement();
	~MarkerManagement();
};

