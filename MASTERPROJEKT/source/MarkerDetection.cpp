#include "MarkerDetection.h"
using namespace cv;



MarkerDetection::MarkerDetection()
{
}


MarkerDetection::~MarkerDetection()
{
}

int MarkerDetection::runMarkerDetection(Mat &frame)
{
	Mat colorThresImg;
	detectedRects.clear();
	markedCorners.clear();
	colorThresImg = colorThreshold(frame);
	std::vector<RotatedRect>  box = getOBB(colorThresImg.clone());
	if (!box.empty()) {
		Mat cornerThresImg = getCornerThresholdImage(frame);
		for (int i = 0; i < box.size(); i++)
		{
			unsigned char corner = detectMarkedCorner(box[i], colorThresImg, cornerThresImg);
			if (corner < 5) {
				markedCorners.push_back(corner);
				detectedRects.push_back(box[i]);
			}
			else return 0;
		}
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


Mat MarkerDetection::getCornerThresholdImage(Mat &frame) {
	Mat CornerThresImage;
	inRange(frame, Scalar(30, 100, 100), Scalar(55, 255, 255), CornerThresImage);

	int erosion_size = 3;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(CornerThresImage, CornerThresImage, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(CornerThresImage, CornerThresImage, MORPH_CLOSE, element, Point(-1, -1));
	return CornerThresImage;
}

unsigned char MarkerDetection::detectMarkedCorner(RotatedRect rect, Mat &frame, Mat &CornerThresImage)
{
	Point2f cornerPoints[4];
	rect.points(cornerPoints);
	unsigned char markedId = 255;

	double min, max;

	for (unsigned int i = 0; i < 4; i++)
	{
		Mat circleimg(CornerThresImage.rows, CornerThresImage.cols, CV_8UC1, Scalar(0, 0, 0));
		circle(circleimg, cornerPoints[i], rect.size.height / 3.5, Scalar(255, 255, 255), -1);
		bitwise_and(circleimg, CornerThresImage, circleimg);
		double min, max;
		minMaxIdx(circleimg, &min, &max);
		if (max == 255) {
			markedId = i;
			circle(frame, cornerPoints[i], rect.size.height / 3.5, Scalar(255, 0, 255), 2);
			break;
		}
	}
	return markedId;
}
