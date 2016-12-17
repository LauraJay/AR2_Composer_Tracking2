#pragma once
#include <queue>
#include <array>
#include <Marker.h>
#include <IdMapping.h>
#include <PlaneCalibration.h>
class MarkerManagement
{
private:
	void deleteMarker(int id);
	void registerNewMarker(cv::RotatedRect normRect, int arucoID, cv::Point2f anglePoint);
	int findMatchID(int arucoID);
	bool readCameraParameters(std::string filename, cv::Mat3f & lut);
	cv::Mat camMatrix;
	cv::Mat distCoeffs;
	
protected:
	//cv::Size markerSize;
	cv::Size frameSize;
	std::queue <int> openIDQueue;
	std::vector<int> takenIDVec;
	std::array<Marker*, 100> trackedMarker;

public:
	void CurrentMarkerWAruco(Marker* tm, cv::RotatedRect normRect, int arucoID, cv::Point2f anglePoint);
	void CurrentMarker(Marker* tm, cv::RotatedRect normRect);
	std::array<Marker*, 100> getTrackedMarker();
	std::vector<int> getTakenIDVec();
	void trackMarker(std::vector<cv::RotatedRect> rect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds, cv::Size size);
	MarkerManagement(cv::Size frameSize, PlaneCalibration::planeCalibData pcd);
	~MarkerManagement();
};