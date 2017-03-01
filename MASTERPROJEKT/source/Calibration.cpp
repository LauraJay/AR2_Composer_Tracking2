#include "Calibration.h"

Calibration::~Calibration()
{
	delete pc;
	delete pe;
}

Calibration::Calibration()
{
	pc = new PlaneCalibration();
	pe = new PoseEstimation();
}

void Calibration::testNewCalib(uEye_input* uei) {
	int ret = pe->generateCamMatAndDistMat(uei);
	cv::Mat P = pe->P;

	std::vector< cv::Point3d > rvecs, tvecs, calibMarkerPos2D;

	////TVEC
	//tvecs.push_back(cv::Point3d(-0.431922, 0.206468, 7.928111));
	//tvecs.push_back(cv::Point3d(0.320648, -0.242227, 7.568028));
	//tvecs.push_back(cv::Point3d(0.353393, 0.226807, 8.281686));
	//tvecs.push_back(cv::Point3d(-0.39238, 0.158182, 8.066885));




	////RVEC
	//rvecs.push_back(cv::Point3d(2.058372, -2.007122, 0.284319));
	//rvecs.push_back(cv::Point3d(-2.818774, -0.028501, -0.268524));
	//rvecs.push_back(cv::Point3d(-3.059901, -0.004356, 0.053008));
	//rvecs.push_back(cv::Point3d(-2.372131, 2.366717, -0.294296));
	//second.push_back(cv::Point3d(3.335211, 0.172952, 0.180908));
	//second.push_back(cv::Point3d(-2.003321, -1.943969, -0.275266));




	std::vector< cv::Point3f> cP;
	cP.push_back(cv::Point3f(-0.001835227, -0.3682832, 0.776637));
	cP.push_back(cv::Point3f(0.7285104, 0.1965812, 0.8005528));
	cP.push_back(cv::Point3f(0.7645366, -0.2706032, 0.8258376));
	cP.push_back(cv::Point3f(0.0856148, -0.1982772, 0.6568241));

	int nMarkers = tvecs.size();
	std::vector<cv::Point3d> transformedPixelCoords;
	for (size_t i = 0; i < nMarkers; i++)
	{
		cv::Point3d tempVec;
		cv::Mat Pinv = P.inv();
		cv::Point3d xdach = cv::Point3d(calibMarkerPos2D[i].x,calibMarkerPos2D[i].y,1);
		tempVec.x = Pinv.at<double>(0, 0) * xdach.x + Pinv.at<double>(0, 1) * xdach.y + Pinv.at<double>(0, 2)*xdach.z + Pinv.at<double>(0, 3) * 1;
		tempVec.y = Pinv.at<double>(1, 0) * xdach.x + Pinv.at<double>(1, 1) * xdach.y + Pinv.at<double>(1, 2)*xdach.z + Pinv.at<double>(1, 3) * 1;
		tempVec.z = 0;
		transformedPixelCoords.push_back(tempVec);
	}
	//Frage Cps haben XYZ und transf.. nur XY1
	cv::Mat affTransform;
	std::vector< uchar >  inliers;
	cv::estimateAffine3D(transformedPixelCoords, cP, affTransform, inliers);
	cv::Mat s = cv::Mat::zeros(1, 4, CV_64F);
	s.at<double>(0, 3) = 1.0;
	cv::vconcat(affTransform, s, affTransform);

}

int Calibration::runPoseEstimation(uEye_input* uei)
{
	int ret = pe->generateCamMatAndDistMat(uei);
	//pe->loadSavedDistCoeff();
	////int rep = pe->generateCam2WorldLUT(pcd);
	cv::Mat frame = uei->getCapturedFrame();
	pe->size = frame.size();
	UndistortRectifyMaps = pe->generateUndistortRectifyMap();
	//pc->distCoeffs = pe->getDistCoeffs();
	//pc->camMatrix = pe->getCameraMat();
	/*frame = uei->getCapturedFrame();
	cv::Mat dst=cv::Mat();
	remap(frame, dst, UndistortRectifyMaps[1], UndistortRectifyMaps[2], cv::INTER_LINEAR);
	cv::imshow("undistortedFrame", dst);
	cv::waitKey(1);*/

	return ret;
}

int Calibration::runPlaneCalib()
{
	std::vector< cv::Point3d > rvecs, tvecs;
	



	//TVEC
	tvecs.push_back(cv::Point3d(-0.431922, 0.206468, 7.928111));
	tvecs.push_back(cv::Point3d(0.320648, -0.242227, 7.568028));
	tvecs.push_back(cv::Point3d(0.353393, 0.226807, 8.281686));
	tvecs.push_back(cv::Point3d(-0.39238, 0.158182, 8.066885));
	



	//RVEC
	rvecs.push_back(cv::Point3d(2.058372, -2.007122, 0.284319));
	rvecs.push_back(cv::Point3d(-2.818774, -0.028501, -0.268524));
	rvecs.push_back(cv::Point3d(-3.059901, -0.004356, 0.053008));
	rvecs.push_back(cv::Point3d(-2.372131, 2.366717, -0.294296));
	//second.push_back(cv::Point3d(3.335211, 0.172952, 0.180908));
	//second.push_back(cv::Point3d(-2.003321, -1.943969, -0.275266));


	

	std::vector< cv::Point3f> cP;
	cP.push_back(cv::Point3f(-0.001835227, -0.3682832, 0.776637));
	cP.push_back(cv::Point3f(0.7285104,  0.1965812,0.8005528));
	cP.push_back(cv::Point3f(0.7645366, -0.2706032, 0.8258376));
	cP.push_back(cv::Point3f(0.0856148, -0.1982772, 0.6568241));
	
	
	
	/*cv::Mat invYM = cv::Mat::eye(4, 4, CV_64F);
	cv::Mat invZM = cv::Mat::eye(4, 4, CV_64F);
	invYM.at<double>(1, 1) = -1.;
	invZM.at<double>(2,2) = -1.;*/
	//cv::Mat rotMat=cv::Mat(3, 3, CV_64F);
	//cv::Mat vecR = cv::Mat(3, 1, CV_64F);
	//vecR.at<double>(0, 0) = tVecs[0].x;
	//vecR.at<double>(1, 0) = tVecs[0].y;
	//vecR.at<double>(2, 0) = tVecs[0].y;
	//cv::Rodrigues(vecR, rotMat);
	//cv::Mat transformationM;
	//cv::Mat temp = cv::Mat(3,4,CV_64F);
	//cv::hconcat(rotMat, cv::Mat(second[0]), temp);
	//cv::Mat t = cv::Mat::zeros(1,4,CV_64F);
	//t.at<double>(0, 3) = 1.0;
	//cv::vconcat(temp, t, transformationM);*/



	
	//cv::Mat R;
	//cv::Rodrigues(rvecs[0],R);
	//cv::Mat temp = cv::hconcat(R, cv::Mat(tvecs[0]) , temp);
	//cv::Mat transform;
	//cv::Mat t = cv::Mat(1, 4, CV_64F);
	//t.at<double>(3, 3) = 1.;

	//cv::vconcat(temp, t, transform);

	// detect markers and estimate pose
	
	cv::Mat affTransform ;
	std::vector< uchar >  inliers ;
	cv::estimateAffine3D(tvecs, cP, affTransform, inliers);
	cv::Mat s = cv::Mat::zeros(1,4,CV_64F);
	s.at<double>(0, 3) = 1.0;
	cv::vconcat(affTransform,s, affTransform);
	
	tvecs.push_back(cv::Point3d(0.169287, -0.095240, 7.930131));
	tvecs.push_back(cv::Point3d(0.384523, 0.189636, 7.511865));
	cP.push_back(cv::Point3f(0.488138, 0.09607637, 0.6641259));
	cP.push_back(cv::Point3f(0.7502835, -0.1891577, 0.6582546));



	
	for (int i = 0; i < tvecs.size(); i++)
	{
		cv::Point3d tVec = tvecs[i];
		
		cv::Point3d resVec;

		//cv::Mat affTransform = affTransform /** invZM*transformationM.inv()*invYM*/;
		
		
		resVec.x= affTransform.at<double>(0,0) * tVec.x + affTransform.at<double>(0, 1) * tVec.y + affTransform.at<double>(0, 2)*tVec.z+ affTransform.at<double>(0,3)*1;
		resVec.y= affTransform.at<double>(1, 0) * tVec.x + affTransform.at<double>(1, 1) * tVec.y + affTransform.at<double>(1, 2)*tVec.z + affTransform.at<double>(1, 3) * 1;
		resVec.z = affTransform.at<double>(2, 0) * tVec.x + affTransform.at<double>(2, 1) * tVec.y + affTransform.at<double>(2, 2)*tVec.z + affTransform.at<double>(2, 3) * 1;


	
		printf("Groundtruth:  (%f, %f, %f) \n", cP[i].x, cP[i].y, cP[i].z);
		printf("Transformed Vector: (%f, %f, %f) \n", resVec.x, resVec.y, resVec.z);
		printf("EROOR: %f, %f, %f \n \n", std::abs(resVec.x) - std::abs(cP[i].x), std::abs(resVec.y) - std::abs(cP[i].y), std::abs(resVec.z) - std::abs(cP[i].z));

	}
	return 1;
}



int Calibration::catchPlaneMarker(cv::Mat frame)
{
	int ret = pc->detectAruco(frame);
	return ret;
} 

int Calibration::generatePlaneCalib()
{
	int ret = pc->computePlaneCalibration();
	pcd = pc->getPlaneCalibData();
	return ret;
}

//int Calibration::generateCam2WorldLUT()
//{
//	int rep =1;
//	if(pe->loadCameraMat()&& pe->loadSavedDistCoeff())
//	rep = pe->generateCam2WorldLUT(pcd);
//	return rep;
//}

PlaneCalibration::planeCalibData Calibration::getPlaneCalibData() {
	pcd = pc->getPlaneCalibData();
	if (pcd.size.width == 0 && pcd.size.height==0)pcd = pc->loadImagePlane();
	return pcd;
}

std::vector<cv::Mat> Calibration::getUndistortRectifyMaps()
{
	return UndistortRectifyMaps;
}

std::vector<cv::Mat> Calibration::loadUndistortRectifyMaps()
{

	return pe->loadUndistortRectifyMaps();
}


