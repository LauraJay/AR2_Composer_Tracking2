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
#include <ctime>

#include <uEye_input.h>

	namespace {
		const char* about =
			"Calibration using a ChArUco board\n"
			"  To capture a frame for calibration, press 'c',\n"
			"  If input comes from video, press any key for next frame\n"
			"  To finish capturing, press 'ESC' key and calibration starts.\n";
		const char* keys =
		"{w        |       | Number of squares in X direction }"
			"{h        |       | Number of squares in Y direction }"
			"{sl       |       | Square side length (in meters) }"
			"{ml       |       | Marker side length (in meters) }"
			"{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
			"					DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
			"					DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
			"					DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
			"{@outfile |<none> | Output file with calibrated camera parameters }"
			"{v        |       | Input from video file, if ommited, input comes from camera }"
			"{ci       | 0     | Camera id if input doesnt come from video (-v) }"
			"{dp       |       | File of marker detector parameters }"
			"{rs       | false | Apply refind strategy }"
			"{zt       | false | Assume zero tangential distortion }"
			"{a        |       | Fix aspect ratio (fx/fy) to this value }"
			"{pc       | false | Fix the principal point at the center }"
			"{sc       | false | Show detected chessboard corners after calibration }";
	}



class PoseEstimation {


private:

public:
	
	~PoseEstimation() {};
	PoseEstimation() {};
	int runPoseEstimation(uEye_input* uei);
};