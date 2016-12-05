
#include <Main.h>

//#define VIDEOVERA
//#define VIDEOLAURAALIEN
//#define VIDEOLAURA
#define TCP
//#define logFile
#define uEYE
#define useNotTestClasses

#ifdef logFile
	std::ofstream debugLogFile;
#endif //logFile

Main::~Main(){
	//TODO
}


Main::Main() {
	//TODO
}

#ifdef useNotTestClasses
int main()
{
	std::array<Marker*,100> marker;
	std::vector<int> takenIdVec;
	bool calibSuccess = true;
	int counter = -1;
	cv::Mat frame;
#ifdef TCP
	calibSuccess = false;
	//start TCP
	TCP_output* out = new TCP_output();
	out->startTCPServer();
	//receive data
	int isCalibrated = out->receiveTCPData();
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
	cv::VideoCapture cap("F:/Master/Masterprojekt/Testvideos/02.avi");
	//cv::VideoCapture cap("C:/Users/Vera/Desktop/Aufnahme01.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cap >> frame;
#endif // VIDEOVERA

#ifdef uEYE
	// uEye Caputure
	uEye_input* uei = new uEye_input();
	uei->inituEyeCam();
	frame = uei->getCapturedFrame();

#endif //uEYE
	//first MarkerSize, second Threshold
	MarkerManagement* mm = new MarkerManagement(frame.size());
	cv::namedWindow("edges", cv::WINDOW_NORMAL);
	MarkerDetection* md = new MarkerDetection();
	
	while (true){
		clock_t start, end;
		counter++;
		start = clock();

#ifdef uEYE
		frame = uei->getCapturedFrame();
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
#ifdef TCP
			if (isCalibrated == 0) {
				Calibration* calib = new Calibration();
				calib->runCalibration(true, false, false);
				printf("hrilues");
				PlaneCalibration::planeCalibData pcd = calib->getPlaneCalibData();
				calibSuccess = pcd.success;
				out->setPCD(pcd);
				if (calibSuccess)
					isCalibrated = 1;
			}
#endif // TCP
			if (calibSuccess) {

				//run Marker Detection			
				int sucess = md->runMarkerDetection(frame);
				if (sucess == 1) {
					std::vector<cv::RotatedRect> rects = md->getDetectedRects();
					std::vector<int> arucoIds = md->getArucoIds();
					std::vector<std::vector<cv::Point2f>> corners = md->getArucoCorners();
					/*
						for each (cv::RotatedRect r in rects)
						{
							cv::Point2f vert[4];
							r.points(vert);
							for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
								line(frame, vert[i], vert[(i + 1) % 4], cv::Scalar(255, 0, 255), 1, CV_AA);
							}
					}*/

					//run MarkerManagement

					mm->trackMarker(rects, corners, arucoIds, frame.size());
					marker = mm->getTrackedMarker();
					takenIdVec = mm->getTakenIDVec();
				}
				else {
					marker = mm->getTrackedMarker();
				}
				debug(frame, marker, counter, takenIdVec);
				cv::imshow("edges", frame);
				cv::waitKey(1);
				//printf("frame sec: %f; nMarker: %d, PosX: %f, PosY: %f \n", 1. / z, takenIdVec.size(), marker[takenIdVec[0]]->getCenter().x, marker[takenIdVec[0]]->getCenter().y);
			}
			else break;

#ifdef TCP
			//Send Markerdata via TCP
			out->sendTCPData(marker, takenIdVec);

#endif // TCP
			end = clock();
			float z = end - start;
			z /= CLOCKS_PER_SEC;
			printf("fps: %f\r", z);
		}
	}
	delete md;
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
#endif //useTestClasses

void debug(cv::Mat & frame, std::array<Marker*, 100> marker, int counter, std::vector<int> takenIDVec)
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

			// Draw Boxes
			for (int i = 0; i < vertices.size(); ++i) {
				line(frame, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 255, 255), 1, CV_AA);
			}
		}
	}
#ifdef logFile
	debugLogFile.close();
#endif //logFile
}


