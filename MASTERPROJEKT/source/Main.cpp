#include <Main.h>

#define useTCP
#define useNotTestClasses
#define runDebug

int currentStatus = -1;
int calibStatus = -1;
std::vector<cv::Mat> maps;
int calibSamples = 25;

Main::~Main() {
}

Main::Main() {
}

#ifdef useNotTestClasses
int main()
{
	std::array<Marker*, 100> marker;
	std::vector<int> takenIdVec;
	int counter = -1;
	cv::Mat frame;
	PlaneAndAffineCalibration::planeCalibData pcd;

	uEye_input* uei1 = new uEye_input();
	uei1->inituEyeCam();
	frame = uei1->getCapturedFrame();
	if (frame.empty()) {
		printf("No uEye Camera found. Please check the connection. \n");
		system("pause");
		return EXIT_FAILURE;
	}

	#ifdef useTCP
	    //start 
	    TCP* tcp = new TCP(frame.size());
	    int tcpRep = tcp->startTCPServer();
	    if (tcpRep == 1) { system("pause"); return EXIT_FAILURE; }

	    Calibration* calib = new Calibration();
	    int correspondingPoints = 0;
	    bool PlaneCalibDone = false;
		
	
		while (true) {
			currentStatus = tcp->receiveStatus();
			if (currentStatus == tcp->sceneStart)  break;
			calibStatus = currentStatus;
	
			if (calibStatus == tcp->planeAndPoseCalib) {
				int res = calib->runPoseEstimation(uei1);
				if (res > -1)
					tcp->sendStatus(tcp->PoseCalibDone);
	
				else {
					printf("No Pose Calibration done!\n");
				}
			}
	
			if (calibStatus == tcp->planeOnlyCalib){
				calib->pe->loadCameraMat();
				calib->pc->camMatrix = calib->pe->cameraMatrix;
				calib->pc->distCoeffs = calib->pe->distCoeffs;
		}
			
	        //SPIELFELDKALIBRIERUNG
			if (calibStatus == tcp->planeOnlyCalib || calibStatus == tcp->planeAndPoseCalib) {
				while (!PlaneCalibDone) {
					currentStatus = -1;
					currentStatus = tcp->receiveStatus();
					frame = uei1->getCapturedFrame();
					if (currentStatus == tcp->ControlerButtonPressed) {
						correspondingPoints = calib->catchPlaneMarker(frame);
						if (correspondingPoints <= calibSamples) {
							if (correspondingPoints == -1) {
								tcp->sendStatus(tcp->ArucoNotFound);
							}
							else{
							tcp->sendStatus(tcp->ArucoFound);
							printf("Count of corresponding Points %i:\n",correspondingPoints);
							calib->pc->AllControllerPositions.push_back(tcp->receiveControllerPositions());
							}
						}
						 if (correspondingPoints == calibSamples) {
							printf("Limit of corresponding Points is reached.");
							int rep = calib->generateAffineAndPlaneCalib();
							tcp->sendStatus(tcp->PlaneCalibDone);
							PlaneCalibDone = true;
						}
					}
				}
			}
	
	
	        else if (currentStatus == tcp->reCalib) {
	            maps.clear();
				correspondingPoints = 0;
	            PlaneCalibDone = false;
	            calibStatus = -1;
	            delete calib;
	            calib = new Calibration();
	        }
	    }
	    pcd = calib->getPlaneCalibData();

		if (correspondingPoints == 0) {
			calib->pe->loadCameraMat();
			calib->pc->camMatrix = calib->pe->cameraMatrix;
			calib->pc->distCoeffs = calib->pe->distCoeffs;
			calib->pc->loadAffineTransform();
		}

	#endif 	// TCP
	    //first MarkerSize, second Threshold
		CoordsTransformtion2Untiy* ct2u = new CoordsTransformtion2Untiy();
		ct2u->affineTransform = calib->pc->affTransform;
	
	    MarkerManagement* mm = new MarkerManagement(frame.size(), pcd);
		mm->camMat = calib->pe->cameraMatrix;
		mm->distMat = calib->pe->distCoeffs;
	    MarkerDetection* md = new MarkerDetection();
	    delete calib;

	    while (true) {
	#ifdef runDebug
	        clock_t start, end;
	        counter++;
	        start = clock();
	#endif //runDebug

	        frame = uei1->getCapturedFrame();
	        if (!frame.empty()) {
	          cv::Mat imgDebug = frame.clone();
	            //run Marker Detection			
	            int sucess = md->runMarkerDetection(frame);
	            if (sucess == 1) {
	                std::vector<cv::RotatedRect> rects = md->getDetectedRects();
	                std::vector<int> arucoIds = md->getArucoIds();
	                std::vector<std::vector<cv::Point2f>> corners = md->getArucoCorners();
	
					// FOR DEBUG DETECTED GREEN RECTS
	                //for each (cv::RotatedRect r in rects)
	                //{
	                //    cv::Point2f vert[4];
	                //    r.points(vert);
	                //    for (int i = 0; i < sizeof(vert) / sizeof(cv::Point2f); ++i) {
	                //        line(imgDebug, vert[i], vert[(i + 1) % 4], cv::Scalar(0, 0, 255), 1, CV_AA);
	                //    }
	                //}
	              
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
				ct2u->computeTransformation2Unity(marker,takenIdVec);

	#ifdef useTCP
	            //Send Markerdata via TCP
	            tcp->sendMarkerData(marker, takenIdVec, frame);
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
		delete ct2u;
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



