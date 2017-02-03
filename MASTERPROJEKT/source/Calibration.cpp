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
	cv::Mat dst, image;

	image = cv::imread("Checkerboard.jpg", CV_LOAD_IMAGE_COLOR);
	cv::undistort(image, dst, pe->getCameraMat(), pe->getDistCoeffs());

	//cv::imwrite("UeyeDistCoeffs.jpg", image);

	cv::imshow("undistortedImg", dst);
	cv::waitKey(0);
	return ret;
}


int Calibration::runCameraMatrix(cv::VideoCapture cap)
{

	cv::Mat frame;
	pcd = getPlaneCalibData();
	//int ret =1;
	int ret = pe->runPoseEstimation(cap);
	//if (ret != -1)
		//ret = pe->generateCam2WorldLUT(pcd);

	cv::Mat dst,image;

	image = cv::imread("Checkerboard.jpg", CV_LOAD_IMAGE_COLOR);
	cv::undistort(image, dst, pe->getCameraMat(), pe->getDistCoeffs());
	
	//cv::imwrite("UeyeDistCoeffs.jpg", image);
	
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
	if(pe->loadCameraParameters())
	rep = pe->generateCam2WorldLUT(pcd);
	return rep;
}

PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	pcd = pc->getPlaneCalibData();
	if (pcd.size.width == 0 && pcd.size.height==0)pcd = pc->loadImagePlane();
	return pcd;
}


