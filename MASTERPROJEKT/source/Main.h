#pragma once
#include <opencv2/opencv.hpp>
#include <MarkerDetection.h>
#include <MarkerManagement.h>
#include <IdMapping.h>
#include <Marker.h>
#include <Output.h>



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
void debug(Mat &imageHSV2, std::vector<Marker*> marker, int counter);
std::vector<cv::Point2f> getPixelCoords(std::vector<cv::Point2f>vertices, Point2f center, Size size);
