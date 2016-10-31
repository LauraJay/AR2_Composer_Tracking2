#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <uEye_input.h>
#include <fstream>


#include <MarkerDetection.h>
#include <MarkerManagement.h>
#include <IdMapping.h>
#include <Marker.h>
#include <TCP_output.h>



class Main
{
	
private:
	Main& operator=(const Main&) {};
	Main(const Main&);;

public:
	Main();
	~Main();
};

int main();
void debug(Mat &frame, std::array<Marker*, 200> marker, int counter, std::vector<int> takenIDVec);
std::vector<cv::Point2f> getPixelCoords(std::vector<cv::Point2f>vertices, Point2f center, Size size);
