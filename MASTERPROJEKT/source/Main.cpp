#include <iostream>
#include <opencv2/opencv.hpp>
#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <uEye_input.h>
#include <Main.h>
using namespace cv;
//#define uEYE
//#define VIDEOVERA
#define VIDEOLAURA
//#define TCP

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
	int idOrder[256];
	
#ifdef TCP
	//start TCP
	Output* out = new Output();
	out->startTCPServer();
	int hf = 0;
#endif 	// TCP


#ifdef VIDEOLAURA
	//VideoCapture cap("C:/Users/student/Desktop/Laura/Testmaterial/001_A_Ohne_Verdeckung.avi");
	VideoCapture cap("C:/Users/student/Desktop/Laura/Testmaterial/001_B_Ohne_Verdeckung.avi");
	//VideoCapture cap("C:/Users/student/Desktop/Laura/Testmaterial/002_A_Nichtmarkierte_Ecken_verdeckt.avi");

	if (!cap.isOpened())  // check if we succeeded
		return -1;
#endif // VIDEOLAURA

#ifdef VIDEOVERA
	//Einbindung Video Vera 
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_A_Ohne_Verdeckung.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_B_Ohne_Verdeckung.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/002_A_Nichtmarkierte_Ecken_verdeckt.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/002_B_Nichtmarkierte_Ecken_verdeckt.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/003_A_Markierte_Ecke_verdeckt.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/003_B_Markierte_Ecke_verdeckt.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/004_A_Person_verdeckt_Marker.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/004_B_Person_verdeckt_Marker.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/005_A_Farbige_Aermel.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/005_B_Farbige_Aermel.avi");
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/006_Nacheinander_Hineinschieben.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
#endif // VIDEOVERA



#ifdef uEYE
	// uEye Caputure
	uEye_input* uei = new uEye_input();
	uei->inituEyeCam();
	//namedWindow("Captured Video", 1);
#endif // uEYE


	int counter = -1;
	Mat frame;
#ifdef uEYE
	for (int i = 0; i < 30; i++)
	{
		frame = uei->getCapturedFrame();
	}
#endif //uEYE

	namedWindow("edges", 1);

	while (true)
	{
		counter++;
		printf("COUNTER %i \n ", counter);
#ifdef uEYE
		frame = uei->getCapturedFrame();
#endif // uEYE

		//// Mark Middel for Calibration
		//line(frame, Point2f(630, 512), Point2f(650, 512), Scalar(255, 0, 0), 2, CV_AA);
		//line(frame, Point2f(640, 502), Point2f(640, 522), Scalar(255, 0, 0), 2, CV_AA);
		//cv::imshow("Captured Video", frame);
		//if (cv::waitKey(1) >= 0) break;


#ifdef VIDEOLAURA
		cap >> frame; // get a new frame from camera
#endif // VIDEOLAURA


#ifdef VIDEOVERA
		cap >> frame; // get a new frame from camera
#endif // VIDEOVERA


		if (!frame.empty()) {
			Mat imageHSV2;
			cvtColor(frame, imageHSV2, COLOR_BGR2HSV);
			// run Marker Detection
			MarkerDetection* md = new MarkerDetection();
			//MarkerManagement* mm = new MarkerManagement();
			int sucess = md->runMarkerDetection(imageHSV2);
			if (sucess > 0) {
				std::vector<RotatedRect> rects = md->getDetectedRects();
				std::vector<unsigned char> markedCorners = md->getMarkedCorners();
				//printf("Marked Corners: %i \n",markedCorners.size());
				//run MarkerManagement
				for (int i = 0; i < rects.size(); i++)
				{
					mm->trackMarker(rects[i], markedCorners[i], frame.size());
				}
				marker = mm->getTrackedMarker();
				delete md;
				//delete mm;
				
			}
			//printf("marker Anz: %i \n", marker.size());
			debug(imageHSV2, marker, counter);

			imshow("edges", imageHSV2);
			if (waitKey(4) >= 0)break;
		}

#ifdef TCP
		//Send Markerdata via TCP
		out->sendTCPData(marker);

#endif // TCP
		//mm->getTrackedMarker().empty();
	}
#ifdef uEYE
	uei->exitCamera();
	delete uei;
#endif // uEYE


	delete mm;


#ifdef TCP
	delete out;
#endif // TCP


	return EXIT_SUCCESS;
}


void debug(Mat &imageHSV2, std::vector<Marker*> marker, int counter) {

	//_____________________________________________________________________________________________________________________________________//
	//For Debugging
	//Mat debug = imageHSV2.clone();
	// Print Frame Number
	counter++;
	std::ostringstream os2;
	os2 << counter;
	String s2 = os2.str();
	putText(imageHSV2, s2, Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);

	for (int k = 0; k < marker.size(); k++) {
		Marker* m = marker[k];
		std::vector<cv::Point2f>vertices;
		vertices = m->getPoints();
		int id = m->getId();
		printf("\tid: %d\n", id);
		float angle = m->getAngle();
		Point2f c = m->getCenter();
		c.x = c.x*imageHSV2.size().width;
		c.y = c.y*imageHSV2.size().height;
		vertices = getPixelCoords(vertices, c, imageHSV2.size());
		// Print ID to BoxCenter
		std::ostringstream os;
		os << id;
		String s = os.str();

		putText(imageHSV2, s, c, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);

		// Draw Boxes
		for (int i = 0; i < sizeof(vertices) / sizeof(Point2f); ++i) {
			line(imageHSV2, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 255), 1, CV_AA);
		}
	}
}


std::vector<cv::Point2f> getPixelCoords(std::vector<cv::Point2f>vertices, Point2f center, Size size) {
	for (int i = 0; i < vertices.size(); i++)
	{
		int x, y;
		x = vertices[i].x*size.width;
		y = vertices[i].y*size.height;
		vertices.at(i).x = x;
		vertices.at(i).y = y;
	}


	return vertices;
}