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
	PlaneCalibration::planeCalibData pcd;
	int getAngleThreshold(cv::Point2f center);
	
	
protected:
	//cv::Size markerSize;
	cv::Size frameSize;
	std::queue <int> openIDQueue;
	std::vector<int> takenIDVec;
	std::array<Marker*, 100> trackedMarker;

public:
	cv::Mat camMat = cv::Mat(3,3,CV_64F);
	cv::Mat distMat = cv::Mat(1, 5, CV_64F);
	void CurrentMarkerWAruco(Marker* tm, cv::RotatedRect normRect, int arucoID, cv::Point2f anglePoint, bool isConstant);
	void CurrentMarker(Marker* tm, cv::RotatedRect normRect);
	std::array<Marker*, 100> getTrackedMarker();
	std::vector<int> getTakenIDVec();
	void trackMarker(std::vector<cv::RotatedRect> rect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds, cv::Size size);
	MarkerManagement(cv::Size frameSize, PlaneCalibration::planeCalibData pcd);
	~MarkerManagement();
	std::vector< cv::Point3d > estimateGreenRects(cv::RotatedRect r);
};