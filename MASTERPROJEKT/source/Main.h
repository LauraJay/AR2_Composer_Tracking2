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
#include <time.h> 

#include <MarkerDetection.h>
#include <MarkerManagement.h>
#include <IdMapping.h>
#include <Marker.h>
#include <TCP_output.h>
#include <Calibration.h>

class Main
{
	
private:
	Main& operator=(const Main&) {};
	Main(const Main&);

public:
	Main();
	~Main();
};

int main();
void debug(cv::Mat &frame, std::array<Marker*, 100> marker, int counter, std::vector<int> takenIDVec);
