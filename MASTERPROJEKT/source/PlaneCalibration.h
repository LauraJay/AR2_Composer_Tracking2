#pragma once
#include <opencv2/opencv.hpp>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/aruco.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


class PlaneCalibration {
public:
	~PlaneCalibration() {};
	PlaneCalibration();
	struct planeCalibData {
		bool success;
		cv::Point2f upperCorner;
		cv::Point2f lowerCorner;
		cv::Size size;
	};
	planeCalibData pcd;
	planeCalibData getPlaneCalibData();
	int computePlaneCalibration();
	int detectAruco(cv::Mat frame);
	int PlaneCalibration::getSizeOfMarkerPos();
private:
	std::vector<cv::Point2f>markerPositions;
	cv::Point2f upperLeft;
	cv::Point2f lowerLeft;
	cv::Point2f upperRight;
	cv::Point2f lowerRight;
	int dictionaryId;
	float markerLength;
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	void initAruco();
	std::vector<int> arucoIds;
	planeCalibData PlaneCalibration::readPlaneCalibrationFile();
	std::fstream calibFile;

protected:
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> rejected;

};