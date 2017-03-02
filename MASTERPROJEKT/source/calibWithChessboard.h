#pragma once
#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"

#include <uEye_input.h>

class calibWithChessboard {

private:

public:
	cv::Mat cameraMatrix, distCoeffs;
	std::vector<cv::Mat> rvecs, tvecs;
	~calibWithChessboard() {};
	calibWithChessboard() {};
	int runCalibWithChessboard(uEye_input* uei);
};