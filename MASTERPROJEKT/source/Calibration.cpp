#include "Calibration.h"

int Calibration::runCalibration(cv::Mat frame, bool doPlaneCalib, bool doPoseEstimation, bool doMarkerSizeCalib)
{
	int ret = 0;
	if (doPlaneCalib) {
		PlaneCalibration* pc = new PlaneCalibration();
		ret = pc->runPlaneCalibration(frame);
		if (ret == 0)
			pcd = pc->getPlaneCalibData();
		delete pc;
	}
	/*if (doPoseEstimation) {
		PoseEstimation* pe = new PoseEstimation();
		ret = pe->runPoseEstimation(uei);
		delete pe;
	}*/

	/*MarkerSizeCalibration* msc = new MarkerSizeCalibration();
	ret = msc->runMarkerSizeCalibration();
	delete msc;
	*/
	//uei->exitCamera();
	//delete uei;
	return 0;
}

PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	return pcd;
}


