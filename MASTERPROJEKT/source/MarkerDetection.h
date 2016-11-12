#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace cv;
class MarkerDetection
{
private:
	Mat frame;
	Mat colorThreshold(Mat &frame);
	std::vector<RotatedRect> detectedRects;
	std::vector<int> arucoIds;
	std::vector<RotatedRect> getOBB(Mat colorThresImg);
	std::vector<RotatedRect> detectMarkerRectangles(Mat frame);
	void initArucoParams();
	void detectArucoMarker(Mat frame);

public:
	MarkerDetection();
	~MarkerDetection();
	int runMarkerDetection(Mat &frame);
	std::vector<RotatedRect> getDetectedRects();
	std::vector<int> getArucoIds();
};

