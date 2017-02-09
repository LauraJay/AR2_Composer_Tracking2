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
	//int ret =1;
	int ret = pe->generateCamMatAndDistMat(uei);
	pe->loadSavedDistCoeff();
	int rep = pe->generateCam2WorldLUT(pcd);
	
	// For Debug
	cv::Mat  map1, map2;
	initUndistortRectifyMap(pe->getCameraMat(), pe->getDistCoeffs(), cv::Mat(),
		cv::getOptimalNewCameraMatrix(pe->getCameraMat(), pe->getDistCoeffs(), uei->getCapturedFrame().size(), 1, uei->getCapturedFrame().size(), 0),
		uei->getCapturedFrame().size(), CV_16SC2, map1, map2);

	cv::Mat dst, image;
	image = cv::imread("Checkerboard.jpg", CV_LOAD_IMAGE_COLOR);
	remap(image, dst, map1, map2, cv::INTER_LINEAR);
	//cv::undistort(image, dst, pe->getCameraMat(), pe->getDistCoeffs());

	cv::imwrite("UeyeDistCoeffs.jpg", dst);
	cv::imshow("undistortedImg", dst);
	cv::waitKey(0);

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


