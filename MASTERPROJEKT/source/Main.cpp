#include <Main.h>

//#define VIDEOVERA
#define useTCP
#define uEYE
#define useNotTestClasses
#define runDebug

//int statusWhileRun = -1;
int currentStatus = -1;
int calibStatus = -1;
std::vector<cv::Mat> maps;

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
	//runSizeMeasure(uei1);
	//system("pause");
	//runSizeMeasure(uei1);
	//system("pause");
	//runSizeMeasure(uei1);
	//system("pause");
	//runSizeMeasure(uei1);
	//system("pause");

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

	while (true) {
		currentStatus = tcp->receiveStatus();
		if (currentStatus == tcp->sceneStart)  break;
		calibStatus = currentStatus;
		printf("calibstatus: %d \n", calibStatus);

		if (calibStatus == tcp->planeAndPoseCalib) {
			int res = calib->runPoseEstimation(uei1);
			maps = calib->getUndistortRectifyMaps();
			if (res > -1 && maps.size() == 2)
				tcp->sendStatus(tcp->PoseCalibDone);

			else {
				printf("No Pose Calibration done!\n");
				/* system("pause");
				 return EXIT_FAILURE;*/
			}
		}

		/*if (calibStatus == tcp->planeOnlyCalib){
			maps = calib->loadUndistortRectifyMaps();
	}*/
		
        //SPIELFELDKALIBRIERUNG
        if (calibStatus == tcp->planeOnlyCalib || calibStatus == tcp->planeAndPoseCalib) {
            while (!PlaneCalibDone) {
                currentStatus = -1;
                currentStatus = tcp->receiveStatus();
                    frame = uei1->getCapturedFrame();
                    //frame = getCalibratedFrame(frame);
                if (currentStatus == tcp->ControlerButtonPressed) {
                   // printf("Controller Status : %d \n", currentStatus);
            
                    numOfPlaneCorners = calib->catchPlaneMarker(frame);
                    printf("numOfPlaneCorners: %d \n", numOfPlaneCorners);
                    switch (numOfPlaneCorners) {
                    case 1: 
						tcp->sendStatus(tcp->ArucoFound1); break;
					case 2:
						tcp->sendStatus(tcp->ArucoFound2); break;
                    case 3: {tcp->sendStatus(tcp->ArucoFound3);
                        int rep = calib->generatePlaneCalib();
						std::vector<float> sizes =calib->pc->markerSize;
                        //if (calibStatus == tcp->planeOnlyCalib)
                        if (rep > -1 && sizes.size() == 3) {
							tcp->sendDistMarkerCamera(sizes[0],sizes[1],sizes[2]);//Entfernungen als float Werte einfügen!
                            tcp->sendStatus(tcp->PlaneCalibDone);
                            //printf("PCD: UR : %f, %f ; LL: %f,%f \n", calib->getPlaneCalibData().upperCorner.x, calib->getPlaneCalibData().upperCorner.y, calib->getPlaneCalibData().lowerCorner.x, calib->getPlaneCalibData().lowerCorner.y);
                            PlaneCalibDone = true;
                        }
                        else {
                            printf("No Plane Calibration done!\n");
                            /*system("pause");
                            return EXIT_FAILURE;*/
                        }                        
                    }break;   
                    default:tcp->sendStatus(tcp->ArucoNotFound); break;
                    } 
                }
            }
        }


        else if (currentStatus == tcp->reCalib) {
            maps.clear();
            numOfPlaneCorners = 0;
            PlaneCalibDone = false;
            calibStatus = -1;

            delete calib;
            calib = new Calibration();
            //cv::destroyWindow("undistortedImg");
        }
    }
    /*printf("load LUT ...\n");
    tcp->loadLUT();
    printf("...finished LUT loading \n");*/
    pcd = calib->getPlaneCalibData();
    //printf("PCD: up : %f, %f ; lp: %f,%f \n", calib->getPlaneCalibData().upperCorner.x, calib->getPlaneCalibData().upperCorner.y, calib->getPlaneCalibData().lowerCorner.x, calib->getPlaneCalibData().lowerCorner.y);
    tcp->setPCD(pcd);
    delete calib;

#endif 	// TCP
    //first MarkerSize, second Threshold
    MarkerManagement* mm = new MarkerManagement(frame.size(), pcd);
    MarkerDetection* md = new MarkerDetection();

    while (true) {
#ifdef runDebug
        clock_t start, end;
        counter++;
        start = clock();
#endif //runDebug

#ifdef uEYE
        frame = uei1->getCapturedFrame();
       // frame = getCalibratedFrame(frame);
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

                for each (cv::RotatedRect r in rects)
                {
                    cv::Point2f vert[4];
                    r.points(vert);
                    for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
                        line(imgDebug, vert[i], vert[(i + 1) % 4], cv::Scalar(0, 0, 255), 1, CV_AA);
                    }
                }
              
                mm->trackMarker(rects, corners, arucoIds, frame.size());
                marker = mm->getTrackedMarker();
                takenIdVec = mm->getTakenIDVec();
            }
            else {
                marker = mm->getTrackedMarker();
            }
#ifdef runDebug
           imgDebug = debug(imgDebug, marker, counter, takenIdVec, cv::Rect(pcd.upperCorner, pcd.lowerCorner));
		   cv::namedWindow("debug", cv::WINDOW_KEEPRATIO);
		   cv::imshow("debug", imgDebug);
            cv::waitKey(1);
#endif //runDebug

#ifdef useTCP
            //Send Markerdata via TCP
            tcp->sendMarkerData(marker, takenIdVec, frame);
           // statusWhileRun= tcp->receiveTCPData();
#endif // TCP_connection
#ifdef runDebug
            end = clock();
            float z = end - start;
            z /= CLOCKS_PER_SEC;
           // printf("fps: %f\r", 1 / z);
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
    rectangle(frame, r, cv::Scalar(0, 0, 255), 5);



   /* int x1 = frame.size().width*(1. / 6);
    int y1 = frame.size().height*(1. / 6);*/
   /* int x2 = frame.size().width*(2. / 6);
    int y2 = frame.size().height*(2. / 6);*/
    /*cv::Rect unsharp = cv::Rect(cv::Point(x2, y2), cv::Point(frame.size().width - x2, frame.size().height - y2));
    rectangle(frame, unsharp, cv::Scalar(0, 255, 0), 2);*/
    /*cv::Rect unsharp2 = cv::Rect(cv::Point(x1, y1), cv::Point(frame.size().width - x1, frame.size().height - y1));
    rectangle(frame, unsharp2, cv::Scalar(0, 255, 0), 2);
*/

    for each (int id in takenIDVec)
    {
        cv::RotatedRect r = marker[id]->getRect();
        r.angle = marker[id]->getAngle();
        Marker* m = marker[id];
        cv::Point2f vert[4];
       r.points(vert);
        for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
            line(frame, vert[i], vert[(i + 1) % 4], cv::Scalar(255, 255, 255), 1, CV_AA);
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

cv::Mat getCalibratedFrame(cv::Mat frame) {
    cv::Mat calibFrame = frame.clone();
    if (maps.size() == 2)
        remap(frame, calibFrame, maps[0], maps[1], cv::INTER_LINEAR);
    return calibFrame;

}

void runSizeMeasure(uEye_input * uei1)
{
	int counter1 = 0;
	int numOfPlaneCorners = 0;
	cv::Mat frame;
	Calibration* calib = new Calibration();
	int samples = 20;
	while (counter1 != samples) {
		frame = uei1->getCapturedFrame();
		cv::imshow("test", frame);
		cv::waitKey(1);
		numOfPlaneCorners = calib->catchPlaneMarker(frame);
		counter1++;
	}
	counter1 = 0;
	std::vector<float> sizes = calib->pc->markerSize;
	float mw = 0.;
	for (size_t i = 0; i < sizes.size(); i++)
	{
		mw += sizes[i];
	}
	mw /= samples;
	printf("Mittelwert %f\n", mw);

	//Varianz var
	double var = 0;
	for (int i = 0; i < sizes.size(); i++)
	{
		var += (sizes[i] - mw) * (sizes[i] - mw);
	}
	var /= (sizes.size() - 1);

	//Standardabweichung sigma
	double sigma = sqrt(var);

	printf("Varianz: %f\n", var);
	printf("Standardabweichung: %f\n", sigma);

	delete calib;
}