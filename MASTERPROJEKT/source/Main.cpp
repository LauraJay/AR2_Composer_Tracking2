#include <Main.h>

//#define VIDEOVERA
#define useTCP
#define uEYE
#define useNotTestClasses
#define runDebug

int currentStatus = -1;
int calibStatus = -1;

Main::~Main() {
	//TODO
}

Main::Main() {
	//TODO
}

#ifdef useNotTestClasses
int main()
{
	std::array<Marker*, 100> marker;
	std::vector<int> takenIdVec;
	int counter = -1;
	cv::Mat frame;
	PlaneCalibration::planeCalibData pcd;
#ifdef uEYE
	uEye_input* uei1 = new uEye_input();
	uei1->inituEyeCam();
	frame = uei1->getCapturedFrame();
	if (frame.empty()) {
		printf("No uEye Camera found. Please check the connection. \n");
		system("pause");
		return EXIT_FAILURE;
	}
#endif // uEYE
#ifdef VIDEOVERA
	//Einbindung Video Vera 
	cv::VideoCapture cap("C:/Users/Vera/Desktop/3.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cap >> frame;
#endif // VIDEOVERA
#ifdef useTCP
	//start 
	// uEye Caputure
	TCP* tcp = new TCP(frame.size());
	int tcpRep = tcp->startTCPServer();
	if (tcpRep == 1) { system("pause"); return EXIT_FAILURE; }
	Calibration* calib = new Calibration();
	int numOfPlaneCorners = 0;
	bool PlaneCalibDone = false;

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
				int res = calib->runPoseEstimation(uei1);
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
					system("pause");
					return EXIT_FAILURE;
				}
			}
		}
		currentStatus = tcp->receiveTCPData();
	}
	printf("load LUT ...");
	tcp->loadLUT();
	printf("...finished LUT loading");
	pcd = calib->getPlaneCalibData();
	tcp->setPCD(pcd);
	delete calib;

#endif 	// TCP

	//first MarkerSize, second Threshold
	MarkerManagement* mm = new MarkerManagement(frame.size(), pcd);
	cv::namedWindow("debug", cv::WINDOW_NORMAL);
	MarkerDetection* md = new MarkerDetection();

	while (true) {
#ifdef runDebug
		clock_t start, end;
		counter++;
		start = clock();
#endif //runDebug

#ifdef uEYE
		frame = uei1->getCapturedFrame();
#endif // uEYE

#ifdef VIDEOVERA
		cap >> frame; // get a new frame from camera
#endif // VIDEOVERA
		if (!frame.empty()) {
			cv::Mat imgDebug = frame.clone();
			//run Marker Detection			
			int sucess = md->runMarkerDetection(frame);
			if (sucess == 1) {
				std::vector<cv::RotatedRect> rects = md->getDetectedRects();
				std::vector<int> arucoIds = md->getArucoIds();
				std::vector<std::vector<cv::Point2f>> corners = md->getArucoCorners();
				mm->trackMarker(rects, corners, arucoIds, frame.size());
				marker = mm->getTrackedMarker();
				takenIdVec = mm->getTakenIDVec();
			}
			else {
				marker = mm->getTrackedMarker();
			}
#ifdef runDebug
			imgDebug = debug(imgDebug, marker, counter, takenIdVec, cv::Rect(pcd.upperCorner, pcd.lowerCorner));
			cv::imshow("debug", imgDebug);
			cv::waitKey(1);
#endif //runDebug

#ifdef useTCP
			//Send Markerdata via TCP
			tcp->sendTCPData(marker, takenIdVec);
#endif // TCP_connection
#ifdef runDebug
			end = clock();
			float z = end - start;
			z /= CLOCKS_PER_SEC;
			printf("fps: %f\r", 1 / z);
#endif //runDebug
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

cv::Mat debug(cv::Mat & frame, std::array<Marker*, 100> marker, int counter, std::vector<int> takenIDVec, cv::Rect r)
{
	// Print Frame Number
	counter++;
	std::ostringstream os2;
	os2 << counter;
	cv::String s2 = os2.str();
	putText(frame, s2, cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);
	rectangle(frame, r, cv::Scalar(0, 0, 255));

	for each (int id in takenIDVec)
	{
		Marker* m = marker[id];
			cv::Point2f vert[4];
			m->getRect().points(vert);
			for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
				line(frame, vert[i], vert[(i + 1) % 4], cv::Scalar(255, 0, 255), 1, CV_AA);
		if (m->isVisible() == 1) {
			std::vector<cv::Point2f>vertices;
			vertices = m->getPoints();
			int id = m->getId();
			float angle = m->getAngle();
			cv::Point2f c = m->getCenter();

			// Print ID to BoxCenter
			std::ostringstream os;
			os << id;
			cv::String s = os.str();

			putText(frame, s, c, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);
			circle(frame, m->getCenter(), 4, cv::Scalar(0, 255, 0));

			}
		}


	}
	return frame;
}



