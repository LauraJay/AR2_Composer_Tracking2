#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;



Mat colorThreshold(Mat imageHSV) {
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

std::vector<RotatedRect>  getOBB(Mat image) {
	std::vector<Point> points;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Canny(image, image, 50, 150, 3);
	findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<RotatedRect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = minAreaRect(Mat(contours_poly[i]));
	}
	return boundRect;
}

float getOrientation(RotatedRect rect, Mat hsvImage) {
	Point2f cornerPoints[4];
	rect.points(cornerPoints);
	Mat outputYellow;
	inRange(hsvImage, Scalar(30, 0, 0), Scalar(55, 255, 255), outputYellow);

	int erosion_size = 2;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(outputYellow, outputYellow, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(outputYellow, outputYellow, MORPH_CLOSE, element, Point(-1, -1));
	
	float angleRad;
	float angleGrad;
	const double PI(3.14159265);
	Point2f unitVector(100, 0);
	for (unsigned int i = 0; i < 4; i++)
	{
		Mat circleimg(outputYellow.rows, outputYellow.cols, CV_8UC1, Scalar(0, 0, 0));
		circle(circleimg, cornerPoints[i], rect.size.height / 2, Scalar(255, 255, 255), -1);
		bitwise_and(circleimg, outputYellow, circleimg);
		double min, max;
		minMaxIdx(circleimg, &min, &max);
		if (max == 255) {
			//Case 1: Green Corner lies above the center of the box
			if(cornerPoints[i].y <= rect.center.y){
			Point2f orientationVector = cornerPoints[i] -rect.center;
			//Calculate the angle between the unit vector and the vector between the center and the green corner
			float scalar = (unitVector.x * orientationVector.x) + (unitVector.y * orientationVector.y);
			float distance1 = sqrt((unitVector.x * unitVector.x) + (unitVector.y * unitVector.y));
			float distance2 = sqrt((orientationVector.x * orientationVector.x) + (orientationVector.y * orientationVector.y));
			angleRad= acos(scalar / (distance1 * distance2));
			angleGrad = angleRad * 180 / PI;
			break;
			}
			//Case 1: Green Corner lies below the center of the box
			if (cornerPoints[i].y > rect.center.y) {
				Point2f orientationVector = cornerPoints[i] - rect.center;
				//Calculate the angle between the unit vector and the vector between the center and the green corner
				float scalar = (unitVector.x * orientationVector.x) + (unitVector.y * orientationVector.y);
				float distance1 = sqrt((unitVector.x * unitVector.x) + (unitVector.y * unitVector.y));
				float distance2 = sqrt((orientationVector.x * orientationVector.x) + (orientationVector.y * orientationVector.y));
				angleRad = acos(scalar / (distance1 * distance2));
				angleGrad =360 - ( angleRad * 180 / PI);
				break;
			}
		}

		//_______________________________________________________________________________________________________________________________//
		// For Debug
		// Print CornerPoints
		//std::ostringstream os;
		//os << i;
		//String s = os.str();
		//putText(hsvImage, s, cornerPoints[i], FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);
		////Circle Green Edges
		//if (max == 255) {
		//	circle(hsvImage, cornerPoints[i], rect.size.height / 2, Scalar(255, 255, 255), 1);
		//	break;
		//}
	}
	return angleGrad;
}

int main()
{

	//Einbindung Video Laura 
	//VideoCapture cap("C:/Users/Laura/Documents/Master/Masterprojekt/Testbilder/02_Videos/001_A_Ohne_Verdeckung.avi");

	//Einbindung Video Vera 
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_A_Ohne_Verdeckung.avi
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_B_Ohne_Verdeckung.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/002_A_Nichtmarkierte_Ecken_verdeckt.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/002_B_Nichtmarkierte_Ecken_verdeckt.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/003_A_Markierte_Ecke_verdeckt.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/003_B_Markierte_Ecke_verdeckt.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/004_A_Person_verdeckt_Marker.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/004_B_Person_verdeckt_Marker.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/005_A_Farbige_Aermel.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/005_B_Farbige_Aermel.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/006_Nacheinander_Hineinschieben.avi");

	if (!cap.isOpened())  // check if we succeeded
		return -1;


	namedWindow("edges", 1);
	int counter = 0;
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
		std::vector<RotatedRect>  box2 = getOBB(colorthre2);
		std::vector<float> orientation;
		Mat test(colorthre2.rows, colorthre2.cols, CV_8UC1, Scalar(0, 0, 0));
		for (int i = 0; i < box2.size(); i++)
		{
			orientation.push_back(getOrientation(box2[i], imageHSV2));
		}

		//_____________________________________________________________________________________________________________________________________//
		//For Debugging
		Mat debug = imageHSV2.clone();
		// Print Frame Number
		counter++;
		std::ostringstream os2;
		os2 << counter;
		String s2 = os2.str();
		putText(debug, s2, Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);

		for (int k = 0; k < box2.size(); k++) {
			Point2f vertices[4];
			RotatedRect box = box2[k];
			box.points(vertices);
			float f = orientation[k];
			// Print Angle to BoxCenter
			/*std::ostringstream os;
			os << f;
			String angle = os.str();
			
			putText(debug, angle, box.center, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);*/
			// Draw Boxes
			for (int i = 0; i < sizeof(vertices) / sizeof(Point2f); ++i) {
				line(debug, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 255), 1, CV_AA);
			}
		}
		imshow("edges", debug);
		if (waitKey(45.625) >= 0) break;
	}
	return EXIT_SUCCESS;
}


