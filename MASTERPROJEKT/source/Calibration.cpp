#include "Calibration.h"

Calibration::~Calibration()
{
	delete pc;
	delete pe;
}

Calibration::Calibration()
{
	pc = new PlaneAndAffineCalibration();
	pe = new PoseEstimation();
}
 // calls all relevent methods and attributes for pose estimation
//@param uEye_input* uei: object of ueye camera
//@return int: 1 is sucess and -1 not sucessful
int Calibration::runPoseEstimation(uEye_input* uei)
{
	int ret = pe->generateCamMatAndDistMat(uei);
	pc->distCoeffs = pe->getDistCoeffs();
	pc->camMatrix = pe->getCameraMat();
	return ret;
}


// calls Aruco Marker dectection of current marker for affine Transformation
//@param cv::Mat frame: current frame of uEye
//@return int: 1 is sucess and -1 not sucessful
int Calibration::catchPlaneMarker(cv::Mat frame)
{
	int ret = pc->detectAruco(frame);
	return ret;
} 

// calls all relevent methods and attributes to generate the plane data and the affine Transform
//@return int: 1 is sucess and -1 not sucessful
int Calibration::generateAffineAndPlaneCalib()
{
	int ret = pc->computePlaneCalibration();
	//pc->debugCamMatImg();
	int rep = pc->computeAffineTransformation();
	bool saved = pc->saveAffineTransform();
	pc->printControllerPosError();
	pcd = pc->getPlaneCalibData();
	return ret;
}


// returns plane data struct
//@return PlaneAndAffineCalibration::planeCalibData: data of game plane
PlaneAndAffineCalibration::planeCalibData Calibration::getPlaneCalibData() {
	pcd = pc->getPlaneCalibData();
	if (pcd.size.width == 0 && pcd.size.height==0)pcd = pc->loadImagePlane();
	return pcd;
}







