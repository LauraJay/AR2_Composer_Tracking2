#include "Calibration.h"

Calibration::~Calibration()
{
	delete pc;
	delete pe;
}

Calibration::Calibration()
{
	pc = new PlaneCalibration();
	pe = new PoseEstimation();
}

int Calibration::runCameraMatrix(uEye_input* uei)
{
	//int ret =1;
	int ret = pe->runPoseEstimation(uei);
	if(ret != -1)
	ret = pe->generateCam2WorldLUT();
	return ret;
}

int Calibration::catchPlaneMarker(cv::Mat frame)
{
	int ret = pc->detectAruco(frame);
	return ret;
}

int Calibration::generatePlaneCalib()
{
	int ret = pc->computePlaneCalibration();
	pcd = pc->getPlaneCalibData();
	return ret;
}

int Calibration::generateCam2WorldLUT()
{
	int rep =1;
	if(pe->loadCameraParameters())
	rep = pe->generateCam2WorldLUT();
	return rep;
}

PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	return pcd;
}


