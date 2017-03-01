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
	std::vector<cv::Point2f> imagePlane;
	cv::Mat1d cameraMatrix, distCoeffs;
	cv::Mat rotationMatrix;
	std::vector<double>  tvec;
	std::vector<cv::Mat> rvecs, tvecs;
	//void computeCamera2World(PlaneCalibration::planeCalibData pcd);
	std::vector<cv::Point2f> getImagePlane(PlaneCalibration::planeCalibData pcd);
	//cv::Mat3f computeCamera2WorldLut();
	//bool saveLUT( const cv::Mat3f lut);
	//cv::Point3f computeWordCoordinates(cv::Point2f uv, cv::Mat rotationMatrix, cv::Mat cameraMatrix, cv::Mat tvec);
	bool saveMaps(const std::vector<cv::Mat> maps);

public:
	cv::Mat screen2Cam;
	~PoseEstimation() {};
	PoseEstimation();
	bool loadInstrincts();
	cv::Size size;
	cv::Mat P;
	//std::vector<double>tvec;
	//bool loadCameraMat();
	//int  generateCam2WorldLUT(PlaneCalibration::planeCalibData pcd);
	int generateCamMatAndDistMat(uEye_input* uei);
	cv::Mat getCameraMat();
	cv::Mat getDistCoeffs();
	std::vector<cv::Mat> generateUndistortRectifyMap();
	//bool loadSavedDistCoeff();
	std::vector<cv::Mat> loadUndistortRectifyMaps();
	bool saveCameraParams(const std::string &filename,
		const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, const std::vector<cv::Mat>rvecs, const std::vector<cv::Mat> tvecs);
};