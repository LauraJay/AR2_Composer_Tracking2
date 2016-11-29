#include "Calibration.h"

int Calibration::runCalibration(bool doPlaneCalib, bool doPoseEstimation, bool doMarkerSizeCalib)
{
	uEye_input* uei = new uEye_input();
	uei->inituEyeCam();
	cv::Mat frame = uei->getCapturedFrame();
	
	int ret = 0;
	if (doPlaneCalib) {
		PlaneCalibration* pc = new PlaneCalibration();
		ret = pc->runPlaneCalibration(frame);
		if (ret)
			planeCorners = pc->getPlaneCorners();
		delete pc;
	}

	PoseEstimation* pe = new PoseEstimation();
	ret = pe->runPoseEstimation();
	delete pe;

	MarkerSizeCalibration* msc = new MarkerSizeCalibration();
	ret = msc->runMarkerSizeCalibration();

	delete msc;
	uei->exitCamera();
	delete uei;
	return 0;
}
