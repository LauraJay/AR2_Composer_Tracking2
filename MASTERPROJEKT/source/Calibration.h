#pragma once
#include <opencv2/opencv.hpp>
#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <PlaneCalibration.h>
#include <PoseEstimation.h>
#include <MarkerSizeCalibration.h>
#include <uEye_input.h>


class Calibration {

private:
	PlaneCalibration::planeCalibData pcd;
	uEye_input uei;
	PoseEstimation* pe;
	std::vector<cv::Mat>UndistortRectifyMaps;
	
public:
	PlaneCalibration* pc;
	~Calibration();
	Calibration();
	int runPoseEstimation(uEye_input* uei);
	int catchPlaneMarker(cv::Mat frame);
	int generatePlaneCalib();
	//int generateCam2WorldLUT();
	PlaneCalibration::planeCalibData getPlaneCalibData();
	std::vector<cv::Mat> getUndistortRectifyMaps();
	std::vector<cv::Mat> loadUndistortRectifyMaps();
	};