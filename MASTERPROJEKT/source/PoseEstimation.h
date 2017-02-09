#pragma once
#include <opencv2/opencv.hpp>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>

#include <uEye_input.h>
#include <PlaneCalibration.h>


class PoseEstimation {


private:
	std::vector<cv::Point2f> imagePlane;
	cv::Mat1d cameraMatrix, distCoeffs;
	cv::Mat rotationMatrix;
	cv::Mat tvec;
	cv::Size size;
	void computeCamera2World(PlaneCalibration::planeCalibData pcd);
	std::vector<cv::Point2f> getImagePlane(PlaneCalibration::planeCalibData pcd);
	cv::Mat3f computeCamera2WorldLut();
	bool saveLUT( const cv::Mat3f lut);
	cv::Point3f computeWordCoordinates(cv::Point2f uv, cv::Mat rotationMatrix, cv::Mat cameraMatrix, cv::Mat tvec);

public:
	~PoseEstimation() {};
	PoseEstimation();
	bool loadCameraMat();
	int  generateCam2WorldLUT(PlaneCalibration::planeCalibData pcd);
	int generateCamMatAndDistMat(uEye_input* uei);
	cv::Mat getCameraMat();
	cv::Mat getDistCoeffs();
	int runPoseEstimation(cv::VideoCapture cap);
	bool loadSavedDistCoeff();
	bool saveCameraParams(const std::string & filename, cv::Size imageSize, float aspectRatio, int flags, const cv::Mat & cameraMatrix, const cv::Mat & distCoeffs, double totalAvgErr);
};