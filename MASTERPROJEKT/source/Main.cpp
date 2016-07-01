
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;



Mat colorThreshold(Mat imageHSV) {
	Mat output;
	Mat outputYellow;


	inRange(imageHSV, Scalar(75, 0, 0), Scalar(130, 255, 255), output);
	inRange(imageHSV, Scalar(38, 0, 0), Scalar(75, 255, 255), outputYellow);
	bitwise_or(output, outputYellow, output);

	int erosion_size = 1;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(output, output, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(output, output, MORPH_CLOSE, element, Point(-1, -1));

	return output;
}

std::vector<RotatedRect>  getOBB(Mat image) {
	std::vector<Point> points;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Canny(image, image, 50, 150, 3);
	findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<RotatedRect> boundRect(contours.size());
	
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = minAreaRect(Mat(contours_poly[i]));
		}


	
	//waitKey(0);
	return boundRect;
}

float getOrientation(RotatedRect rect, Mat hsvImage) {
	Size2f tempSize = rect.size;
	rect.size.height = rect.size.height*0.8f;
	rect.size.width = rect.size.width*0.8f;
	Point2f center = rect.center;
	Size2f size = rect.size;
	Point2f upperLeft;
	Point2f upperRight;
	Point2f bottomLeft;
	Point2f bottomRight;
	Point2f cornerPoints[4];
	rect.points(cornerPoints);

	//order of corners

	//0=bottomLeft
	//1=upperLeft
	//2=upperRight
	//3=bottomRight

	float cornerAngle;
	for (unsigned int i = 0; i < 4; i++)
	{
		bool isMarkedCorner = false;
		Vec3b intensity = hsvImage.at<Vec3b>(cornerPoints[i].y, cornerPoints[i].x);
		uchar h = intensity.val[0];
		if (h >= 38 && h < 75)
			isMarkedCorner = true;

		if (isMarkedCorner) {
			cornerAngle = i*90.f;
			break;
		}
	}
	rect.size = tempSize;
	return cornerAngle + std::abs(rect.angle);

}

int main()
{

	//Einbindung Video
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/014_Multi_35_21.88_45.625_G60.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	
	namedWindow("edges", 1);
	while (true)
	{
		Mat frame;

			cap >> frame; // get a new frame from camera
			if (frame.empty()) {
				break;
			}
			Mat imageHSV2;
			cvtColor(frame, imageHSV2, COLOR_BGR2HSV);
			Mat colorthre2 = colorThreshold(imageHSV2);
			////// image nur zum Debuggen

			std::vector<RotatedRect>  box2 = getOBB(colorthre2);
			std::vector<float> orientation;
			for (int i = 0; i < box2.size(); i++)
			{
				// FEHLER !!!!!
				orientation.push_back(getOrientation(box2[i], imageHSV2));
			}
			////For Debugging

			//Mat debug = imageHSV2.clone();

			//for (int k = 0; k < box2.size(); k++) {

			//	Point2f vertices[4];
			//	RotatedRect box = box2[k];
			//	box.points(vertices);
			//	float f = orientation[k];
			//	std::ostringstream os;
			//	os << f;
			//	String s = os.str();
			//	putText(debug, s,box.center,FONT_HERSHEY_SIMPLEX,1,Scalar(255,255,255),1,8,false);
			//	for (int i = 0; i < sizeof(vertices) / sizeof(Point2f); ++i) {
			//		line(debug, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 255), 1, CV_AA);
			//	}
			//}
			imshow("edges", imageHSV2);
			if (waitKey(45.625) >= 0) break;
	}
	
	
	return EXIT_SUCCESS;
}