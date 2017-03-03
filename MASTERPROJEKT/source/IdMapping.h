#pragma once
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <PlaneAndAffineCalibration.h>
#include <MarkerManagement.h>

class IdMapping{

private:
	bool computeBarycentricTest(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f p);
	
public:
	std::vector<cv::Point2f>  CalculateMotionVectorCenter(cv::RotatedRect normRect, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec);
	int isConstantMarker(std::vector<cv::Point2f> motionCenterVecs,   std::array<Marker*, 100> trackedMarker,std::vector<int> takenIDVec, int arucoID);
	int isTranslatedMarker(std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec, int arucoID);
	int isMarkerOutOfField(Marker * m, PlaneAndAffineCalibration::planeCalibData pcd);
	int hasArucoID(cv::RotatedRect normRect, std::vector<std::vector<cv::Point2f>> corners);
	int isRectOutOfField(cv::RotatedRect r, PlaneAndAffineCalibration::planeCalibData pcd);
	IdMapping() {};
	~IdMapping() {};
};
