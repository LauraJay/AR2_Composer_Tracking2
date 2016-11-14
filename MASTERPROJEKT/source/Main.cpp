
#include <Main.h>

#define VIDEOVERA
//#define VIDEOLAURAALIEN
//#define VIDEOLAURA
//#define TCP
//#define logFile
#define useNotTestClasses

#ifdef logFile
	ofstream debugLogFile;
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

	MarkerManagement* mm = new MarkerManagement();

	std::array<Marker*,200> marker;
	std::vector<int> takenIdVec;

	int counter = -1;
	cv::Mat frame;
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
	cv::VideoCapture cap("C:/Users/Vera/Desktop/Aufnahme01.avi");
	if (!cap.isOpened())  // check if we succeeded
		return -1;
#endif // VIDEOVERA

#ifdef uEYE
	// uEye Caputure
	uEye_input* uei = new uEye_input();
	uei->inituEyeCam();

	for (int i = 0; i < 30; i++)
	{
		frame = uei->getCapturedFrame();
	}
#endif //uEYE

	cv::namedWindow("edges", 1);

	while (true)
	{
		counter++;

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
			// run Marker Detection
			MarkerDetection* md = new MarkerDetection();
			int sucess = md->runMarkerDetection(frame);
			if (sucess == 1) {
				std::vector<cv::RotatedRect> rects = md->getDetectedRects();
				std::vector<int> arucoIds =md->getArucoIds();
				std::vector<std::vector<cv::Point2f>> corners = md->getArucoCorners();
				delete md;
				printf("FrameNumer: %d ; ", counter);
				printf("N Rects: %d \n",rects.size());
				for each (cv::RotatedRect r in rects)
				{
					cv::Point2f vert[4];
					r.points(vert);

					for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
						line(frame, vert[i], vert[(i + 1) % 4], cv::Scalar(255, 0, 255), 1, CV_AA);
					}
			}

				//run MarkerManagement
				if (counter == 79)
					printf("");
				mm->trackMarker(rects,corners,arucoIds,frame.size());
				marker = mm->getTrackedMarker();
				takenIdVec = mm->getTakenIDVec();

				}
			else { marker = mm->getTrackedMarker(); }
			debug(frame, marker, counter,takenIdVec);
			cv::imshow("edges", frame);
			if (cv::waitKey(4) >= 0)break;
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

	delete mm;


#ifdef TCP
	delete out;
#endif // TCP
	return EXIT_SUCCESS;
}
#endif //useTestClasses

void debug(cv::Mat & frame, std::array<Marker*, 200> marker, int counter, std::vector<int> takenIDVec)
{
#ifdef logFile
	debugLogFile.open("debugOutput.txt", ios::out | ios::app);
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
		std::vector<cv::Point2f>vertices;
		vertices = m->getPoints();
		int id = m->getId();
		float angle = m->getAngle();
		cv::Point2f c = m->getCenter();
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
		cv::String s = os.str();

		putText(frame, s, c, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);

		// Draw Boxes
		for (int i = 0; i < vertices.size(); ++i) {
			line(frame, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 255, 255), 1, CV_AA);
		}
	}
#ifdef logFile
	debugLogFile.close();
#endif //logFile
}


std::vector<cv::Point2f> getPixelCoords(std::vector<cv::Point2f>vertices, cv::Point2f center, cv::Size size) {
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