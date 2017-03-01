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
#include <calibWithChessboard.h>

class PoseEstimation {


private:
	std::vector<cv::Mat> rvecs, tvecs;
	cv::Size size;
	bool saveMaps(const std::vector<cv::Mat> maps);

public:
	std::vector<cv::Point2f> imagePlane;
	cv::Mat1d cameraMatrix, distCoeffs;
	cv::Mat rotationMatrix;
	cv::Mat P;
	~PoseEstimation() {};
	PoseEstimation();
	bool loadCameraMat();
	int generateCamMatAndDistMat(uEye_input* uei);
	cv::Mat getCameraMat();
	cv::Mat getDistCoeffs();
	std::vector<cv::Mat> generateUndistortRectifyMap();
	bool loadSavedDistCoeff();
	std::vector<cv::Mat> loadUndistortRectifyMaps();
	bool saveCameraParams(const std::string &filename, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, const std::vector<cv::Mat>rvecs, const std::vector<cv::Mat> tvecs);
};