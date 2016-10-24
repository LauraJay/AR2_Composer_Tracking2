#pragma once
#include <Marker.h>
#include <IdMapping.h>
using namespace cv;
class MarkerManagement
{
private:
	std::vector<Marker*> trackedMarker;
	bool isMarkerOutOfField();
	void deleteMarker(int id);

public:
	void CurrentMarker(Marker* tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	void registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	std::vector<Marker*> getTrackedMarker();
	std::vector<Point2f> normalizeRectPoints(RotatedRect rect, Size size);
	void trackMarker(RotatedRect rect, unsigned char markedCorner, Size size);
	//void firstInit(int id, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	MarkerManagement();
	~MarkerManagement();
};

