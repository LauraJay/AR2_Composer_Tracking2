#pragma once
#include <opencv2/opencv.hpp>
#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <Marker.h>


class uEye_input {

private:
	HIDS hCam;
	int img_width;
	int img_height;
	cv::Mat frame;
	
public:

	uEye_input();

	~uEye_input();

	int inituEyeCam();
	cv::Mat getCapturedFrame();

	int exitCamera();

};