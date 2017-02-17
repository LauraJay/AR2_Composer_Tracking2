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

int Calibration::runPoseEstimation(uEye_input* uei)
{
	int ret = pe->generateCamMatAndDistMat(uei);
	pe->loadSavedDistCoeff();
	//int rep = pe->generateCam2WorldLUT(pcd);
	UndistortRectifyMaps = pe->generateUndistortRectifyMap();
	// For Debug
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
	if(pe->loadCameraMat()&& pe->loadSavedDistCoeff())
	rep = pe->generateCam2WorldLUT(pcd);
	return rep;
}

PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	pcd = pc->getPlaneCalibData();
	if (pcd.size.width == 0 && pcd.size.height==0)pcd = pc->loadImagePlane();
	return pcd;
}

std::vector<cv::Mat> Calibration::getUndistortRectifyMaps()
{
	return UndistortRectifyMaps;
}

std::vector<cv::Mat> Calibration::loadUndistortRectifyMaps()
{

	return pe->loadUndistortRectifyMaps();
}



