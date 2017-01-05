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

public:
	~Calibration() {};
	Calibration() {};
	int runCalibration(bool doPlaneCalib, bool doPoseEstimation, bool doMarkerSizeCalib, int positionCount);
	PlaneCalibration::planeCalibData getPlaneCalibData();
};