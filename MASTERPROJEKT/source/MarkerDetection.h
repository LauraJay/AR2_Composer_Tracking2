#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;
class MarkerDetection
{
private:
	Mat frame;
	Mat colorThreshold(Mat &frame);
	std::vector<RotatedRect> detectedRects;
	std::vector<unsigned char> markedCorners;
	std::vector<RotatedRect> getOBB(Mat colorThresImg);
	unsigned char detectMarkedCorner(RotatedRect rect, Mat &frame, Mat &CornerThresImage);
	Mat getCornerThresholdImage(Mat &frame);

public:
	MarkerDetection();
	~MarkerDetection();
	int runMarkerDetection(Mat &frame);
	std::vector<RotatedRect> getDetectedRects();
	std::vector<unsigned char> getMarkedCorners();
};

