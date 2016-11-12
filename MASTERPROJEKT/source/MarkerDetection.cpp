#include "MarkerDetection.h"
using namespace cv;



MarkerDetection::MarkerDetection()
{
	initArucoParams();
}


MarkerDetection::~MarkerDetection()
{
}

int MarkerDetection::runMarkerDetection(Mat &frame)
{
	detectedRects.clear();
	arucoIds.clear();

	std::vector<RotatedRect>  box = detectMarkerRectangles(frame);
	detectArucoMarker(frame);
	
	if (!box.empty()) {
		for (int i = 0; i < box.size(); i++)
		{
			/*if (corner < 5) {
				markedCorners.push_back(corner);
				detectedRects.push_back(box[i]);
			}*/
			//else return 0;
		}
	}
	return 1;
}

std::vector<RotatedRect> MarkerDetection::getDetectedRects()
{


	return detectedRects;
}

std::vector<int> MarkerDetection::getArucoIds()
{
	return arucoIds;
}

Mat MarkerDetection::colorThreshold(Mat &frame) {
	Mat output;
	inRange(frame, Scalar(55, 0, 50), Scalar(110, 255, 255), output);
	int erosion_size = 1;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(output, output, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(output, output, MORPH_CLOSE, element, Point(-1, -1));
	return output;
}

std::vector<RotatedRect>  MarkerDetection::getOBB(Mat colorThresImg) {
	std::vector<Point> points;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Canny(colorThresImg, colorThresImg, 100, 600, 3);
	findContours(colorThresImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<RotatedRect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = minAreaRect(Mat(contours_poly[i]));
	}
	return boundRect;
}

std::vector<RotatedRect> MarkerDetection::detectMarkerRectangles(Mat frame)
{
	Mat colorThresImg = colorThreshold(frame);
	std::vector<RotatedRect>  box = getOBB(colorThresImg.clone());
	return box;
}

void MarkerDetection::initArucoParams()
{
}

void MarkerDetection::detectArucoMarker(Mat frame)
{
}


