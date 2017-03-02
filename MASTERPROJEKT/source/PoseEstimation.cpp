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




int PoseEstimation::generateCamMatAndDistMat(uEye_input * uei) {
	calibWithChessboard* cOCV = new calibWithChessboard();
	cOCV->runCalibWithChessboard(uei);
	cameraMatrix = cOCV->cameraMatrix;
	distCoeffs = cOCV->distCoeffs;
	tvecs = cOCV->tvecs;
	rvecs = cOCV->rvecs;
	distCoeffs = cv::Mat::zeros(1,5,CV_64F);
	saveCameraParams("CameraMatrix.yml", cameraMatrix, distCoeffs, rvecs, tvecs);
	return 1;
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

	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	//if (!rvecs.empty() && !tvecs.empty())
	//{
	//	CV_Assert(rvecs[0].type() == tvecs[0].type());
	//	cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
	//	for (int i = 0; i < (int)rvecs.size(); i++)
	//	{
	//		cv::Mat r = bigmat(cv::Range(i, i + 1), cv::Range(0, 3));
	//		cv::Mat t = bigmat(cv::Range(i, i + 1), cv::Range(3, 6));

	//		CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
	//		CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
	//		//*.t() is MatExpr (not Mat) so we can use assignment operator
	//		r = rvecs[i].t();
	//		t = tvecs[i].t();
	//	}
	//	//cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
	//	fs << "extrinsic_parameters" << bigmat;
	//}

	printf("CamParams Saved\n");
	return true;
}

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
