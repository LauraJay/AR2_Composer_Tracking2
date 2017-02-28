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

int Calibration::runPoseEstimation(uEye_input* uei)
{
	int ret = pe->generateCamMatAndDistMat(uei);
	//pe->loadSavedDistCoeff();
	////int rep = pe->generateCam2WorldLUT(pcd);
	cv::Mat frame = uei->getCapturedFrame();
	pe->size = frame.size();
	UndistortRectifyMaps = pe->generateUndistortRectifyMap();
	pc->screen2Cam = pe->screen2Cam;
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
	cP.push_back(cv::Point3f(-0.001835227, 0.776637, -0.3682832));
	cP.push_back(cv::Point3f(0.7285104, 0.8005528, 0.1965812));
	cP.push_back(cv::Point3f(0.7645366, 0.8258376, -0.2706032));
	cP.push_back(cv::Point3f(0.0856148, 0.6568241, -0.1982772));
	
	
	
	/*cv::Mat invYM = cv::Mat::eye(4, 4, CV_64F);
	cv::Mat invZM = cv::Mat::eye(4, 4, CV_64F);
	invYM.at<double>(1, 1) = -1.;
	invZM.at<double>(2,2) = -1.;
	cv::Mat rotMat=cv::Mat(3, 3, CV_64F);
	cv::Mat vecR = cv::Mat(3, 1, CV_64F);
	vecR.at<double>(0, 0) = 2.058372;
	vecR.at<double>(1, 0) = -2.007122;
	vecR.at<double>(2, 0) = 0.284319;
	cv::Rodrigues(vecR, rotMat);
	cv::Mat transformationM;
	cv::Mat temp = cv::Mat(3,4,CV_64F);
	cv::hconcat(rotMat, cv::Mat(second[0]), temp);
	cv::Mat t = cv::Mat::zeros(1,4,CV_64F);
	t.at<double>(0, 3) = 1.0;
	cv::vconcat(temp, t, transformationM);*/



	
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
	cP.push_back(cv::Point3f(0.488138, 0.6641259, 0.09607637));
	cP.push_back(cv::Point3f(0.7502835, 0.6582546, -0.1891577));



	
	for (int i = 0; i < tvecs.size(); i++)
	{
		cv::Point3d testVec = tvecs[i];
		
		cv::Point3d resVec;

		cv::Mat allTransform = affTransform /** invZM*transformationM.inv()*invYM*/;
		
		
		resVec.x= allTransform.at<double>(0,0) * testVec.x + allTransform.at<double>(0, 1) * testVec.y + allTransform.at<double>(0, 2)*testVec.z+ allTransform.at<double>(0,3)*1;
		resVec.y= allTransform.at<double>(1, 0) * testVec.x + allTransform.at<double>(1, 1) * testVec.y + allTransform.at<double>(1, 2)*testVec.z + allTransform.at<double>(1, 3) * 1;
		resVec.z = allTransform.at<double>(2, 0) * testVec.x + allTransform.at<double>(2, 1) * testVec.y + allTransform.at<double>(2, 2)*testVec.z + allTransform.at<double>(2, 3) * 1;


	
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


