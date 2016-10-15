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


int main()
{

	MarkerManagement* mm = new MarkerManagement();
	std::vector<Marker*> marker;
	//printf()
	//TCP starten
	Output* out = new Output();
	out->startTCPServer();

	//Einbindung Video Laura 
	VideoCapture cap("C:/Users/student/Desktop/Laura/Testmaterial/001_A_Ohne_Verdeckung.avi");
	

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


	//AUSKOMMENTIERT UM TCP ZU TESTEN
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
		marker = mm->getTrackedMarker();
		//Senden an Luke
		out->sendTCPData(marker);
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
	delete out;
	return EXIT_SUCCESS;
}


