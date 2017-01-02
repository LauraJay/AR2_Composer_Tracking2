#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
//#include <Marker.h>
#include<PlaneCalibration.h>
#include <MarkerManagement.h>

class IdMapping{

private:
	std::ofstream myfile2;
	bool computeBarycentricTest(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f p);
	
public:
	std::vector<cv::Point2f>  CalculateMotionVectorCenter(cv::RotatedRect normRect, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec);
	int isConstantMarker(std::vector<cv::Point2f> motionCenterVecs,   std::array<Marker*, 100> trackedMarker,std::vector<int> takenIDVec, int arucoID);
	int isRotatedMarker(cv::RotatedRect normRect, std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec);
	int isTranslatedMarker(std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec, int arucoID);
	int isMarkerOutOfField(Marker * m, PlaneCalibration::planeCalibData pcd);
	int hasArucoID(cv::RotatedRect normRect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds);
	int isRectOutOfField(cv::RotatedRect r, PlaneCalibration::planeCalibData pcd);
	IdMapping() {};
	~IdMapping() {};
};
