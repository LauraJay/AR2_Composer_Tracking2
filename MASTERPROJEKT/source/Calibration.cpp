#include "Calibration.h"

int Calibration::runCalibration(bool doPlaneCalib, bool doPoseEstimation, bool doMarkerSizeCalib)
{
	uEye_input* uei = new uEye_input();
	uei->inituEyeCamForCalib();
	cv::Mat frame = uei->getCapturedFrame();
	if (doPlaneCalib) {
		int ret = 0;
		printf("Starting plane calibration ... \n");
		printf("Please place the four plane marker on the table. \n");
		PlaneCalibration* pc = new PlaneCalibration();
		while (!frame.empty()) {
			frame = uei->getCapturedFrame();
			ret = pc->runPlaneCalibration(frame);
			if (ret == 0) {
			pcd = pc->getPlaneCalibData();
			break;
			}
		}
		delete pc;
	    printf("Plane calibration finished. \n");
		printf("Please remove the four plane marker. \n");
	}
	if (doPoseEstimation) {
		int ret = 0;
		printf("Starting Pose Estimation ... \n");
		PoseEstimation* pe = new PoseEstimation();
		ret = pe->runPoseEstimation(uei);
		delete pe;
	}

	/*MarkerSizeCalibration* msc = new MarkerSizeCalibration();
	ret = msc->runMarkerSizeCalibration();
	delete msc;*/
	
	uei->exitCamera();
	delete uei;
	return 0;
	}


PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	return pcd;
}


