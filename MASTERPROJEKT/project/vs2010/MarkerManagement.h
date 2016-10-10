#pragma once
#include <F:\Master\Masterprojekt\Git_Workspace\AR2_Composer_Tracking2\MASTERPROJEKT\project\vs2010\Marker.h>
class MarkerManagement
{
private:
	std::vector<Marker*> trackedMarker;
	bool isConstantMarker(std::vector<cv::Point2f> points, unsigned char markedCorner);
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

