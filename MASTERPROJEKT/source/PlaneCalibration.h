#pragma once
#include <opencv2/opencv.hpp>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/aruco.hpp>

class PlaneCalibration {

private:
	cv::Point2f upperLeft;
	cv::Point2f lowerLeft;
	cv::Point2f upperRight;
	cv::Point2f lowerRight;
	int dictionaryId;
	float markerLength;
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	void initAruco();
	int detectAruco(cv::Mat frame);
	std::vector<int> arucoIds;
	int runMarkerDetection(cv::Mat &frame);

protected:
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> rejected;

public:
	~PlaneCalibration() {};
	PlaneCalibration();
	struct planeCalibData {
		bool success;
		cv::Point2f upperLeftCorner;
		cv::Size size;
	};
	planeCalibData getPlaneCalibData();
	int runPlaneCalibration(cv::Mat frame);
};