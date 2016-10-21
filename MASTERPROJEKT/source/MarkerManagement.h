#pragma once
#include <Marker.h>
#include <IdMapping.h>
using namespace cv;
class MarkerManagement
{
private:
	std::vector<Marker*> trackedMarker;
	//bool isConstantMarker(std::vector<Point2f> points, unsigned char markedCorner);
	std::vector<Point2f> normalizeRectPoints(RotatedRect rect, Size size);
	bool isMarkerOutOfField();
	void deleteMarker(int id);
	void registerNewMarker(std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	
public:
	std::vector<Marker*> getTrackedMarker();
	void trackMarker(RotatedRect rect, unsigned char markedCorner,Size size);
	MarkerManagement();
	~MarkerManagement();
};

