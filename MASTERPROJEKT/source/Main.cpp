#include <Main.h>

//#define VIDEOVERA
//#define VIDEOLAURAALIEN
//#define VIDEOLAURA
#define useTCP
//#define logFile
#define uEYE
//#define DebugPoseEsti
#define useNotTestClasses


int currentStatus = -1;
int calibStatus = -1;

#ifdef logFile
std::ofstream debugLogFile;
#endif //logFile

Main::~Main() {
	//TODO
}



Main::Main() {
	//TODO
}



////TESTING
//int main()
//{
//	int x = 0;
//	int counter = 0;
//	uEye_input* uei1 = new uEye_input();
//	uei1->inituEyeCam();
//	cv::Mat frame;
//	cv::Mat frameCap = uei1->getCapturedFrame();
//	while (true) {
//	
//			frameCap = uei1->getCapturedFrame();
//			frame = frameCap.clone();
//		counter++;
//		std::ostringstream os2;
//		os2 << counter;
//		cv::String s2 = os2.str();
//		putText(frame, s2, cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);
//
//		circle(frame, cv::Point(x,150), 7, cv::Scalar(0, 255, 0),2);
//		
//		x+=20;
//		if (x > frame.size().width) x = 0;
//
//		cv::imshow("test", frame);
//		cv::waitKey(1);
//
//	}
//}


#ifdef useNotTestClasses
int main()
{
	std::array<Marker*, 100> marker;
	std::vector<int> takenIdVec;
	//bool calibSuccess = true;

	int counter = -1;
	cv::Mat frame;
	PlaneCalibration::planeCalibData pcd;
#ifdef useTCP
	//start 
	// uEye Caputure
	uEye_input* uei1 = new uEye_input();
	uei1->inituEyeCam();
	frame = uei1->getCapturedFrame();
	TCP* tcp = new TCP(frame.size());
	tcp->startTCPServer();
	Calibration* calib = new Calibration();
	int numOfPlaneCorners = 0;
	bool PlaneCalibDone = false;
	cv::imshow("test", frame);
	cv::waitKey(1);

	while (currentStatus != tcp->sceneStart) {
		calibStatus = tcp->receiveTCPData();
		if (calibStatus == tcp->sceneStart)  break; 

		printf("calibstatus: %d \n", calibStatus);
		//SPIELFELDKALIBRIERUNG
		if (calibStatus == tcp->planeOnlyCalib || calibStatus == tcp->planeAndPoseCalib) {
			while (!PlaneCalibDone) {
				currentStatus = tcp->receiveTCPData();
				if (currentStatus == tcp->ControlerButtonPressed) {
					printf("Controller Status : %d \n", currentStatus);
					frame = uei1->getCapturedFrame();
							cv::imshow("Calib frame", frame);
							cv::waitKey(1);
							numOfPlaneCorners = calib->catchPlaneMarker(frame);
					switch (numOfPlaneCorners) {
					case 1: tcp->sendStatus(tcp->ArucoFound1); break;
					case 2: {tcp->sendStatus(tcp->ArucoFound2);
						int rep = calib->generatePlaneCalib();
						if (rep == -1) printf("Generation of Plane failed. \n");
						PlaneCalibDone = true;
					}
							break;
					default:tcp->sendStatus(tcp->ArucoNotFound);
					}
				}
			}
			//BEIDE KALIBRIERUNGEN
			if (calibStatus == tcp->planeAndPoseCalib) {
				int res = calib->runCameraMatrix(uei1);
				if (res > -1)	tcp->sendStatus(tcp->PoseCalibDone);
				else {
					printf("No Pose Calibration done!");
				}
			}
			// Nur Spielfeld Kalibrierung
			else {
				int res = calib->generateCam2WorldLUT();
				if (res != -1)	tcp->sendStatus(tcp->PlaneCalibDone);
				else {
					printf("No Plane Calibration done!");
				}
			}
		}
		currentStatus = tcp->receiveTCPData();
	}

		tcp->loadLUT();
		pcd = calib->getPlaneCalibData();
		tcp->setPCD(pcd);
	delete calib;

#endif 	// TCP


#ifdef VIDEOLAURA
	cv::VideoCapture cap("C:/Users/AR/Desktop/Laura/05_Testmaterial/02.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cap >> frame;
#endif // VIDEOLAURA

#ifdef VIDEOLAURAALIEN
	VideoCapture cap("C:/Users/student/Desktop/Laura/Testmaterial/001_A_Ohne_Verdeckung.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cap >> frame;
#endif // VIDEOLAURAALIEN

#ifdef VIDEOVERA
	//Einbindung Video Vera 
	cv::VideoCapture cap("C:/Users/Vera/Desktop/1.avi");
	//cv::VideoCapture cap("C:/Users/Vera/Desktop/Aufnahme01.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cap >> frame;
#endif // VIDEOVERA

#ifdef DebugPoseEsti
	Calibration* calib = new Calibration();
	int numOfPlaneCorners = 0;
	int res = calib->runCameraMatrix(uei1);

#endif // DebugPoseEsti




#ifdef uEYE
	// uEye Caputure
	cv::Mat frameCap = uei1->getCapturedFrame();
	frame = uei1->getCapturedFrame();

#endif //uEYE
	//first MarkerSize, second Threshold
	MarkerManagement* mm = new MarkerManagement(frame.size(), pcd);
	cv::namedWindow("edges", cv::WINDOW_NORMAL);
	MarkerDetection* md = new MarkerDetection();

	while (true) {
		clock_t start, end;
		counter++;
		start = clock();

#ifdef uEYE

		frame = uei1->getCapturedFrame();
		//frame = frameCap.clone();
		cv::imshow("frame2", frame);
		cv::waitKey(1);
		/*cv::imshow("frameCap", frameCap);
		cv::waitKey(1);*/
#endif // uEYE

#ifdef VIDEOLAURA
		cap >> frame; // get a new frame from camera
#endif // VIDEOLAURA

#ifdef VIDEOLAURAALIEN
		cap >> frame; // get a new frame from camera
#endif // VIDEOLAURAALIEN

#ifdef VIDEOVERA
		cap >> frame; // get a new frame from camera
#endif // VIDEOVERA
		if (!frame.empty()) {
			cv::Mat imgDebug=frame.clone();
			//run Marker Detection			
			int sucess = md->runMarkerDetection(frame);
			if (sucess == 1) {
				std::vector<cv::RotatedRect> rects = md->getDetectedRects();
				std::vector<int> arucoIds = md->getArucoIds();
				std::vector<std::vector<cv::Point2f>> corners = md->getArucoCorners();

				for each (cv::RotatedRect r in rects)
				{
					cv::Point2f vert[4];
					r.points(vert);
					for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
						line(imgDebug, vert[i], vert[(i + 1) % 4], cv::Scalar(255, 0, 255), 1, CV_AA);
					}
				}

				//run MarkerManagement

				mm->trackMarker(rects, corners, arucoIds, frame.size());
				marker = mm->getTrackedMarker();
				takenIdVec = mm->getTakenIDVec();
			}
			else {
				marker = mm->getTrackedMarker();
			}

		imgDebug = debug(imgDebug, marker, counter, takenIdVec);

			cv::Rect r = cv::Rect(pcd.upperCorner, pcd.lowerCorner);
			rectangle(imgDebug, r, cv::Scalar(0, 0, 255));
			cv::imshow("edges", imgDebug);
			cv::waitKey(1);
			//printf("frame sec: %f; nMarker: %d, PosX: %f, PosY: %f \n", 1. / z, takenIdVec.size(), marker[takenIdVec[0]]->getCenter().x, marker[takenIdVec[0]]->getCenter().y);

#ifdef useTCP
			//Send Markerdata via TCP
			tcp->sendTCPData(marker, takenIdVec);


#endif // TCP_connection
			end = clock();
			float z = end - start;
			z /= CLOCKS_PER_SEC;
			//printf("fps: %f\r", 1 / z);
		}
		else break;
	}
	delete md;
#ifdef uEYE
	uei1->exitCamera();
	delete uei1;
#endif // uEYE

	delete mm;

#ifdef useTCP
	delete tcp;
#endif // TCP_connection
	return EXIT_SUCCESS;

}
#endif //useTestClasses

cv::Mat debug(cv::Mat & frame, std::array<Marker*, 100> marker, int counter, std::vector<int> takenIDVec)
{
#ifdef logFile
	debugLogFile.open("debugOutput.txt", std::ios::out | std::ios::app);
	debugLogFile << "Current Frame " << counter << "\n";
#endif //logFile
	// Print Frame Number
	counter++;
	std::ostringstream os2;
	os2 << counter;
	cv::String s2 = os2.str();
	putText(frame, s2, cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);


	for each (int id in takenIDVec)
	{
		Marker* m = marker[id];
		if (m->isVisible() == 1) {
			std::vector<cv::Point2f>vertices;
			vertices = m->getPoints();
			int id = m->getId();
			float angle = m->getAngle();
			cv::Point2f c = m->getCenter();

#ifdef logFile 
			debugLogFile << "\t ID: " << id << "\n";
			debugLogFile << "\t \t  ANGLE: " << angle << "\n";
			debugLogFile << "\t \t  CENTER X: " << c.x << " CENTER Y: " << c.y << "\n";
#endif //logFile

			// Print ID to BoxCenter
			std::ostringstream os;
			os << id;
			cv::String s = os.str();

			putText(frame, s, c, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);

			circle(frame, m->getCenter(), 4, cv::Scalar(0, 255, 0));
		}


	}


#ifdef logFile
	debugLogFile.close();
#endif //logFile
	return frame;
}



