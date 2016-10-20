#pragma once
#include <Marker.h>
#include <IdMapping.h>
class MarkerManagement
{
private:
	std::vector<Marker*> trackedMarker;
	std::vector<cv::Point2f> rectTOVectorPoints(cv::RotatedRect rect);
	bool isMarkerOutOfField();
	void deleteMarker(int id);
	bool isTrackedMarker(std::vector<cv::Point2f> points, unsigned char markedCorner);
	void registerNewMarker(std::vector<cv::Point2f> rectPoints, cv::Point2f center, unsigned char markedCorner);
public:
	std::vector<Marker*> getTrackedMarker();
	void trackMarker(cv::RotatedRect, unsigned char markedCorner);
	MarkerManagement();
	~MarkerManagement();
};

