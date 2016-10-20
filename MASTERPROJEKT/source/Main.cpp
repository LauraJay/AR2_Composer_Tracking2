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

	////start TCP
	//Output* out = new Output();
	//out->startTCPServer();
	int hf = 0;

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



	if (!cap.isOpened())  // check if we succeeded
		return -1;

	//// uEye Caputure
	////_________________________________________________________________________________________
	//uEye_input* uei = new uEye_input();
	//uei->inituEyeCam();
	namedWindow("edges", 1);
	//namedWindow("Captured Video", 1);
	int counter = -1;
	Mat frame;
	while (true)
	{
		counter++;
		//printf("COUNTER %i \n ", counter);
		//frame = uei->getCapturedFrame();
		/*imshow("Captured Video", frame);
		if (waitKey(1) >= 0) break;*/
		cap >> frame; // get a new frame from camera
		if (!frame.empty()) {
			Mat imageHSV2;
			cvtColor(frame, imageHSV2, COLOR_BGR2HSV);

			// run Marker Detection
			MarkerDetection* md = new MarkerDetection();
			int sucess = md->runMarkerDetection(imageHSV2);
			if (sucess > 0) {
				std::vector<RotatedRect> rects = md->getDetectedRects();
				std::vector<unsigned char> markedCorners = md->getMarkedCorners();
				//printf("Marked Corners: %i \n",markedCorners.size());
				//run MarkerManagement
				for (int i = 0; i < rects.size(); i++)
				{
					mm->trackMarker(rects[i], markedCorners[i],frame.size());
				}

				marker = mm->getTrackedMarker();
				delete md;
			}
			//printf("marker Anz: %i \n", marker.size());
			debug(imageHSV2, marker,counter);
			
		}

			//Send Markerdata via TCP
			//out->sendTCPData(marker);
			mm->getTrackedMarker().empty();
		}

		//uei->exitCamera();
		//delete uei;
		delete mm;
		//delete out;
		return EXIT_SUCCESS;
	}


	void debug(Mat imageHSV2, std::vector<Marker*> marker, int counter) {

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
			Marker* m = marker[k];
			std::vector<cv::Point2f>vertices;
			vertices = m->getPoints();
			int id = m->getId();
			printf("\tid: %d\n", id);
			float angle = m->getAngle();
				//printf("angle: %f\ n", angle);
			Point2f c = m->getCenter();
			//	printf("\PosX: %d\n", c.x);
			//	printf("\PosY: %d\n", c.y);
			getPixelCoords(vertices,c,debug.size());
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
		waitKey(4);
	}


	void getPixelCoords(std::vector<cv::Point2f>vertices, Point2f center, Size size) {
		for each (Point2f p in vertices)
		{
			p.x = p.x*size.width;
			p.y = p.y*size.height;
		}
		center.x = center.x*size.width;
		center.y = center.y*size.height;
	}