#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;
class MarkerDetection
{
private:
	Mat imageHSV;
	Mat cTImg;
	Mat colorThreshold(Mat &imageHSV);
	std::vector<RotatedRect> detectedRects;
	std::vector<unsigned char> markedCorners;
	std::vector<RotatedRect> getOBB(Mat &cTImg);
	unsigned char detectMarkedCorner(RotatedRect rect, Mat &imageHSV, Mat &CornerThresImage);
	Mat getCornerThresholdImage(Mat &imageHSV);
public:
	MarkerDetection();
	~MarkerDetection();
	void runMarkerDetection(Mat &imageHSV);
	std::vector<RotatedRect> getDetectedRects();
	std::vector<unsigned char> getMarkedCorners();
};

