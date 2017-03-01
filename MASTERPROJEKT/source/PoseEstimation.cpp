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

std::vector<cv::Mat> PoseEstimation::generateUndistortRectifyMap()
{
	cv::Mat  map1, map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, size, 1, size),size, CV_16SC2, map1, map2);

	cv::Mat dst, image;
	image = cv::imread("Checkerboard.jpg", CV_LOAD_IMAGE_COLOR);
	remap(image, dst, map1, map2, cv::INTER_LINEAR);
	cv::imshow("undistortedImg", dst);
	cv::waitKey(1);
	cv::imwrite("UeyeDistCoeffs.jpg", dst);
	std::vector<cv::Mat> maps;
	maps.push_back(map1);
	maps.push_back(map2);
	bool saveOK =saveMaps(maps);
	if (!saveOK)printf("Saving UndistortRectifyMap failed.");
	return maps;
}


int PoseEstimation::generateCamMatAndDistMat(uEye_input * uei) {
	calibWithChessboard* cOCV = new calibWithChessboard();
	cOCV->runCalibWithChessboard(uei);
	cameraMatrix = cOCV->cameraMatrix;
	distCoeffs = cOCV->distCoeffs;
	P = cOCV->P;
	tvecs = cOCV->tvecs;
	rvecs = cOCV->rvecs;
	distCoeffs = cv::Mat::zeros(1,5,CV_64F);
	saveCameraParams("CameraMatrix.xml", cameraMatrix, distCoeffs, rvecs, tvecs);
	//loadInstrincts();
	return 1;
}


bool PoseEstimation::loadSavedDistCoeff()
{
	cv::FileStorage fs("DistCoMat.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs [ "distortion_coefficients"] >> PoseEstimation::distCoeffs;
	return true;
}




bool PoseEstimation::saveMaps(const std::vector <cv::Mat> maps) {
	cv::FileStorage fs("Maps.yml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;
	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);
	fs << "calibration_time" << buf;
	fs << "Map1" << maps[0];
	fs << "Map2" << maps[1];
	return true;
}

std::vector <cv::Mat> PoseEstimation::loadUndistortRectifyMaps() {
	std::vector <cv::Mat> maps;
	cv::Mat mat1, mat2;
	cv::FileStorage fs("Maps.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return maps;
	fs["Map1"] >> mat1;
	maps.push_back(mat1);
	fs["Map2"] >> mat2;
	maps.push_back(mat2);
	
	return maps;
}




bool PoseEstimation::saveCameraParams(const std::string &filename, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, const std::vector<cv::Mat>rvecs, const std::vector<cv::Mat> tvecs) {
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;

	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);

	fs << "calibration_time" << buf;

	/*fs << "image_width" << imageSize.width;
	fs << "image_height" << imageSize.height;*/

	//if (flags & cv::CALIB_FIX_ASPECT_RATIO) fs << "aspectRatio" << aspectRatio;

	/*if (flags != 0) {
	sprintf(buf, "flags: %s%s%s%s",
	flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
	flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
	flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
	flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
	}

	fs << "flags" << flags;*/
	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	if (!rvecs.empty() && !tvecs.empty())
	{
		CV_Assert(rvecs[0].type() == tvecs[0].type());
		cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
		for (int i = 0; i < (int)rvecs.size(); i++)
		{
			cv::Mat r = bigmat(cv::Range(i, i + 1), cv::Range(0, 3));
			cv::Mat t = bigmat(cv::Range(i, i + 1), cv::Range(3, 6));

			CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
			CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
			//*.t() is MatExpr (not Mat) so we can use assignment operator
			r = rvecs[i].t();
			t = tvecs[i].t();
		}
		//cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
		fs << "extrinsic_parameters" << bigmat;
	}
	//fs << "distortion_coefficients" << distCoeffs;
	//fs << "avg_reprojection_error" << totalAvgErr;
	printf("CamParams Saved\n");
	return true;
}

bool PoseEstimation::loadCameraMat() {
	cv::FileStorage fs("CameraMatrix.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["camera_matrix"] >> cameraMatrix;
	fs ["image_width"] >> size.width;
	fs [ "image_height" ]>> size.height;
	return true;
}
