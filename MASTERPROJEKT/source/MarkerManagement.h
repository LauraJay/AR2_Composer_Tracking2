#pragma once
#include <queue>
#include <array>
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
	std::queue <int> openIDQueue;
	std::vector<int> takenIDVec;
	std::array<Marker*, 200> trackedMarker;

public:
	void CurrentMarker(Marker* tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	std::array<Marker*, 200> getTrackedMarker();
	std::vector<int> getTakenIDVec();
	void trackMarker(RotatedRect rect, unsigned char markedCorner, Size size);
	MarkerManagement();
	~MarkerManagement();
};

