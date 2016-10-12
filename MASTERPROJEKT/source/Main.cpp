#include <iostream>
#include <opencv2/opencv.hpp>
#include <Main.h>
using namespace cv;

Main::~Main()
{
	//TODO
}


Main::Main() {
	//TODO
}




// alter Stand kann geloescht werden, wenn für LAURA ok
//float getOrientation(RotatedRect rect, unsigned char markedCorner) {
//	
//	Point2f cornerPoints[4];
//	rect.points(cornerPoints);
//	
//	float angleRad;
//	float angleGrad;
//	const double PI(3.14159265);
//	Point2f unitVector(100, 0);
//	
//			//Case 1: Green Corner lies above the center of the box
//			Point2f orientationVector = cornerPoints[markedCorner] -rect.center;
//			//Calculate the angle between the unit vector and the vector between the center and the green corner
//			float scalar = (unitVector.x * orientationVector.x) + (unitVector.y * orientationVector.y);
//			float distance1 = sqrt((unitVector.x * unitVector.x) + (unitVector.y * unitVector.y));
//			float distance2 = sqrt((orientationVector.x * orientationVector.x) + (orientationVector.y * orientationVector.y));
//			angleRad= acos(scalar / (distance1 * distance2));
//			if(cornerPoints[markedCorner].y <= rect.center.y){
//			angleGrad = angleRad * 180 / PI;
//			// TODO Void methode und angle nur in Markerobject
//			}
//			//Case 1: Green Corner lies below the center of the box
//			if (cornerPoints[markedCorner].y > rect.center.y) {
//				Point2f orientationVector = cornerPoints[markedCorner] - rect.center;
//				//Calculate the angle between the unit vector and the vector between the center and the green corner
//				float scalar = (unitVector.x * orientationVector.x) + (unitVector.y * orientationVector.y);
//				//TODO sqrt wirklich notwendig? Könnten wir nicht auch die quardratischen Werte nehmen?
//				float distance1 = sqrt((unitVector.x * unitVector.x) + (unitVector.y * unitVector.y));
//				float distance2 = sqrt((orientationVector.x * orientationVector.x) + (orientationVector.y * orientationVector.y));
//				angleRad = acos(scalar / (distance1 * distance2));
//				angleGrad =360 - ( angleRad * 180 / PI);
//			}
//	return angleGrad;
//		}





int main()
{

	MarkerManagement* mm = new MarkerManagement();
	//Einbindung Video Laura 
	VideoCapture cap("C:/Users/Laura/Documents/Master/Masterprojekt/Testbilder/02_Videos/001_A_Ohne_Verdeckung.avi");

	//Einbindung Video Vera 
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_A_Ohne_Verdeckung.avi
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_B_Ohne_Verdeckung.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/002_A_Nichtmarkierte_Ecken_verdeckt.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/002_B_Nichtmarkierte_Ecken_verdeckt.avi");
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/003_A_Markierte_Ecke_verdeckt.avi");
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

		// run Marker Detection
		MarkerDetection* md = new MarkerDetection();
		md->runMarkerDetection(imageHSV2);
		std::vector<RotatedRect> rects = md->getDetectedRects();
		std::vector<unsigned char> markedCorners = md->getMarkedCorners();
		//run MarkerManagement
		for (int i = 0; i < rects.size(); i++)
		{
		mm->trackMarker(rects[i], markedCorners[i]);
		}
		std::vector<Marker*> marker = mm->getTrackedMarker();
		delete md;
		
		

		//_____________________________________________________________________________________________________________________________________//
		//For Debugging
		Mat debug = imageHSV2.clone();
		// Print Frame Number
		counter++;
		std::ostringstream os2;
		os2 << counter;
		String s2 = os2.str();
		putText(debug, s2, Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);

		for (int k = 0; k < marker.size(); k++) {
		/*	Point2f vertices[4];
			RotatedRect box = rects[k];
			box.points(vertices);*/
			//float f = getOrientation(rects[k],markedCorners[k]);
			Marker* m = marker[k];
			std::vector<cv::Point2f>vertices;
			vertices= m->getPoints();
			int id = m->getId();
			float angle = m->getAngle();
			Point2f c = m->getCenter();

			// Print ID to BoxCenter
			std::ostringstream os;
			os << id;
			String s = os.str();
			
			putText(debug, s, c, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);
		
			//// Draw Boxes
			//for (int i = 0; i < sizeof(vertices) / sizeof(Point2f); ++i) {
			//	line(debug, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 255), 1, CV_AA);
			//}
		}
		imshow("edges", debug);
		if (waitKey(4) >= 0) break;
	}

	delete mm;
	return EXIT_SUCCESS;
}


