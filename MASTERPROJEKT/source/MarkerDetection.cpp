#include "MarkerDetection.h"
#include <opencv2/opencv.hpp>
using namespace cv;



MarkerDetection::MarkerDetection()
{
}


MarkerDetection::~MarkerDetection()
{
}

int MarkerDetection::runMarkerDetection(Mat &imageHSV)
{
	// TODO WORKS NOT CORRECT

	Mat cTImg;
	detectedRects.clear();
	markedCorners.clear();
	//printf("start colorThreshold \n");
	cTImg = colorThreshold(imageHSV);
	//printf("end colorThreshold \n");
	//printf("start getObb \n");
	std::vector<RotatedRect>  box = getOBB(cTImg);
	if (!box.empty()) {
	/*	printf("end getObb \n");
		printf("start cornerThres \n");*/
		Mat cornerThresImg = getCornerThresholdImage(imageHSV);
		//printf("end cornerThres \n");
		//printf("box size: %i \n", box.size());
		//printf("start detect Corners \n");
		for (int i = 0; i < box.size(); i++)
		{
				unsigned char corner = detectMarkedCorner(box[i], imageHSV, cornerThresImg);
				markedCorners.push_back(corner);
				detectedRects.push_back(box[i]);
			
		}
		//printf("end detect Corners \n");
	}
	return 1;
}

std::vector<RotatedRect> MarkerDetection::getDetectedRects()
{
	return detectedRects;
}

std::vector<unsigned char> MarkerDetection::getMarkedCorners()
{
	return markedCorners;
}

Mat MarkerDetection::colorThreshold(Mat &imageHSV) {
	Mat output;
	inRange(imageHSV, Scalar(55, 0, 100), Scalar(110, 255, 255), output);
	int erosion_size = 1;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(output, output, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(output, output, MORPH_CLOSE, element, Point(-1, -1));
	return output;
}

std::vector<RotatedRect>  MarkerDetection::getOBB(Mat &cTImg) {
	std::vector<Point> points;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Canny(cTImg, cTImg, 50, 150, 3);
	findContours(cTImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<RotatedRect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = minAreaRect(Mat(contours_poly[i]));
	}
	return boundRect;
}


Mat MarkerDetection::getCornerThresholdImage(Mat &imageHSV) {
	Mat CornerThresImage;
	inRange(imageHSV, Scalar(30, 0,100), Scalar(55, 255, 255), CornerThresImage);

	int erosion_size = 3;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(CornerThresImage, CornerThresImage, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(CornerThresImage, CornerThresImage, MORPH_CLOSE, element, Point(-1, -1));
	return CornerThresImage;
}

unsigned char MarkerDetection::detectMarkedCorner(RotatedRect rect, Mat &imageHSV, Mat &CornerThresImage)
{
	Point2f cornerPoints[4];
	rect.points(cornerPoints);
	unsigned char markedId=255;

	double min, max;

	for (unsigned int i = 0; i < 4; i++)
	{
		Mat circleimg(CornerThresImage.rows, CornerThresImage.cols, CV_8UC1, Scalar(0, 0, 0));
		circle(circleimg, cornerPoints[i], rect.size.height /2, Scalar(255, 255, 255), -1);
		bitwise_and(circleimg, CornerThresImage, circleimg);
		double min, max;
		minMaxIdx(circleimg, &min, &max);
		if (max == 255) {
			markedId = i;
			circle(imageHSV, cornerPoints[i], rect.size.height / 4, Scalar(255, 0, 255), 2);
			break;
		}
	}
	return markedId;
}
