#pragma once
#include <opencv2/opencv.hpp>
#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <PlaneAndAffineCalibration.h>
#include <PoseEstimation.h>
#include <MarkerSizeCalibration.h>
#include <uEye_input.h>


class Calibration {

private:
	PlaneAndAffineCalibration::planeCalibData pcd;
	uEye_input uei;
	
public:
	PoseEstimation* pe;
	PlaneAndAffineCalibration* pc;
	~Calibration();
	Calibration();
	int runPoseEstimation(uEye_input* uei);
	int catchPlaneMarker(cv::Mat frame);
	int generateAffineAndPlaneCalib();
	PlaneAndAffineCalibration::planeCalibData getPlaneCalibData();
	};