
#include <Main.h>

using namespace cv;
//#define uEYE
#define VIDEOVERA
//#define VIDEOLAURAALIEN
//#define VIDEOLAURA
//#define TCP
//#define logFile
//#define useonlyMarkerDetection
#define useNotTestClasses

#ifdef logFile
	std::ofstream debugLogFile;
#endif //logFile

Main::~Main()
{
	//TODO
}


Main::Main() {
	//TODO
}

#ifdef useNotTestClasses
int main()
{

	std::cout << CV_VERSION << std::endl;
#ifdef useonlyMarkerDetection
	MarkerManagement* mm = new MarkerManagement();

	std::array<Marker*,200> marker;
	std::vector<int> takenIdVec;
#endif // useonlyMarkerDetection

#ifdef TCP
	//start TCP
	TCP_output* out = new TCP_output();
	out->startTCPServer();
#endif 	// TCP


#ifdef VIDEOLAURA
	VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Drehung1.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Drehung2.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Lift1.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Lift2.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Register1.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/reinraus1.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/reinraus2.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Swap1.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/Swap2.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/totalCover1.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/02_Testmaterial/totalCover2.avi");

	//alt
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/01_Testmaterial/001_A_Ohne_Verdeckung.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/01_Testmaterial/001_B_Ohne_Verdeckung.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/01_Testmaterial/006_Nacheinander_Hineinschieben.avi");
	//VideoCapture cap("C:/Users/AR/Desktop/Laura/01_Testmaterial/002_A_Nichtmarkierte_Ecken_verdeckt.avi");

	if (!cap.isOpened())  // check if we succeeded
		return -1;
#endif // VIDEOLAURA

#ifdef VIDEOLAURAALIEN
	VideoCapture cap("C:/Users/student/Desktop/Laura/Testmaterial/001_A_Ohne_Verdeckung.avi");
	

	if (!cap.isOpened())  // check if we succeeded
		return -1;
#endif // VIDEOLAURAALIEN

#ifdef VIDEOVERA
	//Einbindung Video Vera 
	VideoCapture cap("F:/Master/Masterprojekt/Testvideos/001_A_Ohne_Verdeckung.avi");
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
	//VideoCapture cap("F:/Master/Masterprojekt/Testvideos/reinraus1.avi");

	if (!cap.isOpened())  // check if we succeeded
		return -1;
#endif // VIDEOVERA



#ifdef uEYE
	// uEye Caputure
	uEye_input* uei = new uEye_input();
	uei->inituEyeCam();
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
	//LUT aufstellen

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

#ifdef VIDEOLAURAALIEN
		cap >> frame; // get a new frame from camera
#endif // VIDEOLAURAALIEN


#ifdef VIDEOVERA
		cap >> frame; // get a new frame from camera
#endif // VIDEOVERA

		if (!frame.empty()) {
			cvtColor(frame, frame, COLOR_BGR2HSV);
			// run Marker Detection
			MarkerDetection* md = new MarkerDetection();
			int sucess = md->runMarkerDetection(frame);
			if (sucess == 1) {
				std::vector<RotatedRect> rects = md->getDetectedRects();
				//std::vector<unsigned char> markedCorners = md->getMarkedCorners();

#ifdef useonlyMarkerDetection

				// LUT Kalibration des Rechtecks
				//run MarkerManagement

				for (int i = 0; i < rects.size(); i++)
				{
						mm->trackMarker(rects[i], markedCorners[i], frame.size());
				}
				marker = mm->getTrackedMarker();
				takenIdVec = mm->getTakenIDVec();

#endif // useonlyMarkerDetection
				delete md;
			}
#ifdef useonlyMarkerDetection
			else { marker = mm->getTrackedMarker(); }
			debug(frame, marker, counter,takenIdVec);

			imshow("edges", frame);
			if (waitKey(4) >= 0)break;
#endif // useonlyMarkerDetection
		}
		else break;

#ifdef TCP
		//Send Markerdata via TCP
		out->sendTCPData(marker,takenIdVec);

#endif // TCP
	}
#ifdef uEYE
	uei->exitCamera();
	delete uei;
#endif // uEYE
#ifdef useonlyMarkerDetection

	delete mm;
#endif // useonlyMarkerDetection


#ifdef TCP
	delete out;
#endif // TCP
	return EXIT_SUCCESS;
}
#endif //useTestClasses

void debug(Mat & frame, std::array<Marker*, 200> marker, int counter, std::vector<int> takenIDVec)
{
#ifdef logFile
	debugLogFile.open("debugOutput.txt", std::ios::out | std::ios::app);
	debugLogFile << "Current Frame " << counter << "\n";
#endif //logFile
	// Print Frame Number
	counter++;
	std::ostringstream os2;
	os2 << counter;
	String s2 = os2.str();
	putText(frame, s2, Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);
	for each (int id in takenIDVec)
	{
	//for (int k = 0; k < marker.size(); k++) {
		Marker* m = marker[id];
		std::vector<cv::Point2f>vertices;
		vertices = m->getPoints();
		int id = m->getId();
		//printf("\t id: %d\n", id);
		float angle = m->getAngle();
		//printf("\t \t angle: %f\n", angle);
		Point2f c = m->getCenter();
		c.x = c.x*frame.size().width;
		c.y = c.y*frame.size().height;
		vertices = getPixelCoords(vertices, c, frame.size());

#ifdef logFile 
		debugLogFile << "\t ID: " << id << "\n";
		debugLogFile << "\t \t  ANGLE: " << angle << "\n";
		debugLogFile << "\t \t  CENTER X: " << c.x << " CENTER Y: " << c.y << "\n";
#endif //logFile

		// Print ID to BoxCenter
		std::ostringstream os;
		os << id;
		String s = os.str();

		putText(frame, s, c, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1, 8, false);

		// Draw Boxes
		for (int i = 0; i < sizeof(vertices) / sizeof(Point2f); ++i) {
			line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 255), 1, CV_AA);
		}
	}
#ifdef logFile
	debugLogFile.close();
#endif //logFile
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