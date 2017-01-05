#include "Calibration.h"

int Calibration::runCalibration(bool doPlaneCalib, bool doPoseEstimation, bool doCam2WorldLUT, int positionCount)
{
	//Unabhängig Knopfdruck machen
	//Zweimal Ausführung mit positionCount ermöglichen
	int calibStep = positionCount + 1;
	uEye_input* uei = new uEye_input();
	uei->inituEyeCam();
	cv::Mat frame = uei->getCapturedFrame();
	if (doPlaneCalib) {
		int ret = 0;
		printf("Starting plane calibration step \i " , calibStep);
		PlaneCalibration* pc = new PlaneCalibration();
		cv::Mat frame;
		//std::cout << "Press c to capture" << std::endl;
		frame = uei->getCapturedFrame();
		while (!frame.empty()) {
			frame = uei->getCapturedFrame();
			//putText(frame, "Press 'c' to add current frame. 'ESC' to finish and calibrate",
				//cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);

			imshow("out", frame);
			//char key = (char)cv::waitKey(10);
			//if (key == 'c') {
				ret = pc->detectAruco(frame);
				//std::cout << "Frame captured" << std::endl;
			//}
			if (pc->getSizeOfMarkerPos() == 1 ) break;
		}

		ret = pc->computePlaneCalibration();
		if (ret == 1) {
			pcd = pc->getPlaneCalibData();
			printf("Plane calibration finished. \n");
			printf("Please remove the four plane marker. \n");
		}
		else {
			printf("Plane calibration failed. \n");
			printf("Please try again. \n");
		}
		delete pc;
       cvDestroyWindow("out");
	}
	if (doPoseEstimation) {
		int ret = 0;
		printf("Starting Pose Estimation ... \n");
		PoseEstimation* pe = new PoseEstimation();
		ret = pe->runPoseEstimation(uei);
		ret = pe->generateCam2WorldLUT();
		delete pe;
	}

	if (doCam2WorldLUT) {
		PoseEstimation* pe = new PoseEstimation();
		pe->loadCameraParameters();
		int rep = pe->generateCam2WorldLUT();
		delete pe;
	}
	
	uei->exitCamera();
	delete uei;
	return 0;
	}


PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	return pcd;
}


