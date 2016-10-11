#include "MarkerDetection.h"
#include <opencv2/opencv.hpp>
using namespace cv;



MarkerDetection::MarkerDetection()
{
}


MarkerDetection::~MarkerDetection()
{
}

void MarkerDetection::runMarkerDetection(Mat &imageHSV)
{
	// TODO WORKS NOT CORRECT
	detectedRects.clear();
	markedCorners.clear();
	cTImg = colorThreshold(imageHSV);
	std::vector<RotatedRect>  box = getOBB(cTImg);

	for (int i = 0; i < box.size(); i++)
	{
		unsigned char corner = detectMarkedCorner(box[i], imageHSV);
		if (corner < 255)
		{	markedCorners.push_back(corner);
			detectedRects.push_back(box[i]);
		}
	}
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
	inRange(imageHSV, Scalar(30, 0, 0), Scalar(110, 255, 255), output);
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

unsigned char MarkerDetection::detectMarkedCorner(RotatedRect rect, Mat &imageHSV)
{
	Point2f cornerPoints[4];
	rect.points(cornerPoints);
	Mat outputYellow;
	inRange(imageHSV, Scalar(30, 0, 0), Scalar(55, 255, 255), outputYellow);

	int erosion_size = 2;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(outputYellow, outputYellow, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(outputYellow, outputYellow, MORPH_CLOSE, element, Point(-1, -1));
	unsigned char markedId = 255;

	for (unsigned int i = 0; i < 4; i++)
	{
		Mat circleimg(outputYellow.rows, outputYellow.cols, CV_8UC1, Scalar(0, 0, 0));
		circle(circleimg, cornerPoints[i], rect.size.height / 2, Scalar(255, 255, 255), -1);
		bitwise_and(circleimg, outputYellow, circleimg);
		double min, max;
		minMaxIdx(circleimg, &min, &max);
		if (max == 255) {
			markedId = i;
			break;
		}


		//_______________________________________________________________________________________________________________________________//
		// For Debug
		// Print CornerPoints
		//std::ostringstream os;
		//os << i;
		//String s = os.str();
		//putText(imageHSV, s, cornerPoints[i], FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);
		////Circle Green Edges
		//if (max == 255) {
		//	circle(imageHSV, cornerPoints[i], rect.size.height / 2, Scalar(255, 255, 255), 1);
		//	break;
		//}
	}
	return markedId;
}
