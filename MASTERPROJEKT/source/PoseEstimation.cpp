#include "PoseEstimation.h"
#include <iostream>
#include <fstream>

PoseEstimation::PoseEstimation()
{
}

cv::Mat PoseEstimation::getCameraMat()
{
	return cameraMatrix;
}

cv::Mat PoseEstimation::getDistCoeffs()
{
	return distCoeffs;
}



//calls and saves all relevent methods and attributes for intrinsic camera parameters
//@param uEye_input * uei: uEye object
//@return int: 1 if sucess 
int PoseEstimation::generateCamMatAndDistMat(uEye_input * uei) {
	calibWithChessboard* cOCV = new calibWithChessboard();
	cOCV->runCalibWithChessboard(uei);
	cameraMatrix = cOCV->cameraMatrix;
	distCoeffs = cOCV->distCoeffs;
	tvecs = cOCV->tvecs;
	rvecs = cOCV->rvecs;
	distCoeffs = cv::Mat::zeros(1,5,CV_64F);
	saveCameraParams("CameraMatrix.yml", cameraMatrix, distCoeffs);
	return 1;
}


//saves intrinsic camera parameters
//@param const std::string &filename: filename 
//@param cv::Mat &cameraMatrix: camera matrix 
//@param cv::Mat &distCoeffs: distortion coefficients
//@return bool: true if sucessful
bool PoseEstimation::saveCameraParams(const std::string &filename, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs) {
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;

	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);

	fs << "calibration_time" << buf;

	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	printf("CamParams Saved\n");
	return true;
}


//loads intrinsic camera parameters from file
//@return bool: true if sucessful
bool PoseEstimation::loadCameraMat() {
	cv::FileStorage fs("CameraMatrix.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> distCoeffs;
	//fs ["image_width"] >> size.width;
	//fs [ "image_height" ]>> size.height;
	return true;
}
