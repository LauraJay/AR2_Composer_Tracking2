#pragma once
#include <queue>
#include <array>
#include <Marker.h>
#include <IdMapping.h>
class MarkerManagement
{
private:
	cv::RotatedRect normalizeCoord(cv::RotatedRect r, cv::Size size);
	void deleteMarker(int id);
	void registerNewMarker(cv::RotatedRect normRect);
	int findMatchID( int arucoID);
	
protected:

	std::queue <int> openIDQueue;
	std::vector<int> takenIDVec;
	std::array<Marker*, 200> trackedMarker;

public:
	void CurrentMarker(Marker* tm, cv::RotatedRect normRect);
	std::array<Marker*, 200> getTrackedMarker();
	std::vector<int> getTakenIDVec();
	void trackMarker(std::vector<cv::RotatedRect> rect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds, cv::Size size);
	MarkerManagement();
	~MarkerManagement();
};

