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
	cv::Mat P;
	/*int ret = pe->generateCamMatAndDistMat(uei);
	P = pe->P;
	cv::FileStorage fs("P.yml", cv::FileStorage::WRITE);
	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);
	fs << "P" << P;*/
	


	

	//std::vector< cv::Point3f> cP;
	//

	//int nMarkers = tvecs.size();
	//std::vector<cv::Point3d> transformedPixelCoords;
	//for (size_t i = 0; i < nMarkers; i++)
	//{
	//	cv::Point3d tempVec;
	//	cv::Mat Pinv = P.inv();
	//	cv::Point3d xdach = cv::Point3d(calibMarkerPos2D[i].x, calibMarkerPos2D[i].y, 1);
	//	tempVec.x = Pinv.at<double>(0, 0) * xdach.x + Pinv.at<double>(0, 1) * xdach.y + Pinv.at<double>(0, 2)*xdach.z + Pinv.at<double>(0, 3) * 1;
	//	tempVec.y = Pinv.at<double>(1, 0) * xdach.x + Pinv.at<double>(1, 1) * xdach.y + Pinv.at<double>(1, 2)*xdach.z + Pinv.at<double>(1, 3) * 1;
	//	tempVec.z = 0;
	//	transformedPixelCoords.push_back(tempVec);
	//}
	////Frage Cps haben XYZ und transf.. nur XY1
	//cv::Mat affTransform;
	//std::vector< uchar >  inliers;
	//cv::estimateAffine3D(transformedPixelCoords, cP, affTransform, inliers);
	//cv::Mat s = cv::Mat::zeros(1, 4, CV_64F);
	//s.at<double>(0, 3) = 1.0;
	//cv::vconcat(affTransform, s, affTransform);

}

//void Calibration::testNewCalibWithPositions() {
//	cv::Mat cameraMatrix = cv::Mat(3,3,CV_64F);
//	std::vector< cv::Point3f> cP;
//	pe->loadCameraMat();
//
//	cameraMatrix = pe->cameraMatrix;
//	cv::Mat distCoeffs = pe->distCoeffs;
//	
//	std::vector< cv::Point3d > rvecs, tvecs;
//	std::vector< cv::Point2d >	calibMarkerPos2D;
//	std::vector<std::vector<cv::Point2d>> corners;
//
//
//	//Marker Center
//	calibMarkerPos2D.push_back(cv::Point2d(1.20118095e+02, 1.25925995e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(3.49022125e+02, 1.28923950e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(5.90252197e+02, 1.27525932e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(8.37636658e+02, 1.30140182e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(1.15200818e+03, 1.27144821e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(1.31104401e+02, 4.72882904e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(3.48133026e+02, 3.50111420e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(5.61011169e+02, 3.45430023e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(8.09047852e+02, 3.28651154e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(1.14313086e+03, 3.10179138e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(1.60215424e+02, 5.84987793e+02));
//	calibMarkerPos2D.push_back(cv::Point2d(4.10544861e+02, 5.78250000e+02));
//
//
//	//tvecs
//	tvecs.push_back(cv::Point3d(-4.5333910850428416e-01, -3.3619811980412623e-01,2.5659402058558984e+00));
//	tvecs.push_back(cv::Point3d(-2.5640118418035829e-01, -3.3433126805415669e-01,2.5714859180234950e+00));
//	tvecs.push_back(cv::Point3d(-4.7567973751631437e-02, -3.3379074642542383e-01,2.5579824131236495e+00));
//	tvecs.push_back(cv::Point3d(1.6598547647016651e-01, -3.3307306787132812e-01,2.5698482631354427e+00));
//	tvecs.push_back(cv::Point3d(4.3826613026047562e-01, -3.3613708149412969e-01,2.5734721691048277e+00));
//	tvecs.push_back(cv::Point3d(-4.3932277743595666e-01, -3.6350704458339246e-02, 2.5397227729075578e+00));
//	tvecs.push_back(cv::Point3d(-2.5361476822401519e-01, -1.4103042328759760e-01, 2.5359765233886069e+00));
//	tvecs.push_back(cv::Point3d(-7.1604422870014617e-02, -1.4404102319287482e-01, 2.5187547153320522e+00));
//	tvecs.push_back(cv::Point3d(1.3952065781013909e-01, -1.5944616458653516e-01, 2.5377552333627733e+00));
//	tvecs.push_back(cv::Point3d(4.3109975088991348e-01, -1.7789283052866731e-01, 2.5765526757043191e+00));
//	
//
//	//rvecs
//
//	rvecs.push_back(cv::Point3d(3.2904580688184293e+00, 6.9210215512831852e-03,6.1640503531906210e-01));
//	rvecs.push_back(cv::Point3d(3.3369846630107207e+00, 4.2955458395427241e-02,	2.6702165096663466e-01));
//	rvecs.push_back(cv::Point3d(3.1467442406666866e+00, 2.9973792469735094e-02,	8.9127568815449226e-02));
//	rvecs.push_back(cv::Point3d(-3.0351080592151316e+00, -6.4442465095301049e-02, -1.0755395939625219e-01));
//	rvecs.push_back(cv::Point3d(3.2152968919307243e+00, 6.5084417984811266e-02,2.3085579551379698e-01));
//	rvecs.push_back(cv::Point3d(2.9692117712323647e+00, 1.6013421508638548e-02,	5.9650503673456012e-01));
//	rvecs.push_back(cv::Point3d(3.0981013159550770e+00, -6.1734665745526943e-03,3.3943366608463238e-01));
//	rvecs.push_back(cv::Point3d(3.0893271355537939e+00, 6.3132297964859099e-02,	8.0296074054129196e-02));
//	rvecs.push_back(cv::Point3d(3.1724913284575549e+00, 8.5953694012235710e-02,	2.1625851534510104e-01));
//	rvecs.push_back(cv::Point3d(3.2056262593756228e+00, 3.6224526494332568e-02,	2.3396700558669919e-01));
//	
//
//	//controllerPos
//	cP.push_back(cv::Point3d(-0.1164383, 0.6515052, 0.3361615));
//	cP.push_back(cv::Point3d(0.07992923, 0.6529319, 0.3539377));
//	cP.push_back(cv::Point3d(0.2808266, 0.6587553, 0.3800046));
//	cP.push_back(cv::Point3d(0.4895911, 0.6621958, 0.4027203));
//	cP.push_back(cv::Point3d(0.7595708, 0.6668625, 0.4396226));
//	cP.push_back(cv::Point3d(-0.07889199, 0.6423234, 0.04610658));
//	cP.push_back(cv::Point3d(0.09130514, 0.6537876, 0.1725023));
//	cP.push_back(cv::Point3d(0.2788711, 0.6476164, 0.1904057));
//	cP.push_back(cv::Point3d(0.4846785, 0.6533424, 0.229609));
//	cP.push_back(cv::Point3d(0.7637489, 0.6629822, 0.2816445));
//
//
//
//	cv::Mat testImg = cv::Mat::zeros(1280, 1024, CV_8UC3);
//
//	for (int i = 0; i < tvecs.size(); i++)
//	{
//		std::vector< cv::Point3f > axisPoints;
//		axisPoints.push_back(cv::Point3f(0, 0, 0));
//		std::vector< cv::Point2f > imagePoints;
//		cv::InputArray rvec = rvecs[i];
//		projectPoints(axisPoints, rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints);
//		cv::circle(testImg, imagePoints[0], 5, cv::Scalar(0, 255, 0));
//		cv::circle(testImg, calibMarkerPos2D[i], 3, cv::Scalar(255, 0, 0));
//
//	}
//
//	cv::namedWindow("debug", cv::WINDOW_KEEPRATIO);
//	cv::imshow("debug", testImg);
//	cv::waitKey(0);
//
//	cv::Mat affTransform;
//	std::vector< uchar >  inliers;
//	cv::estimateAffine3D(tvecs, cP, affTransform, inliers);
//	cP.push_back(cv::Point3d(-0.0342648, 0.6298499, -0.05447781));
//	cP.push_back(cv::Point3d(0.1713181, 0.6374502, -0.01919568));
//	rvecs.push_back(cv::Point3d(-3.2441970171821874e+00, 4.5887449506315852e-03, -6.3694230525944662e-01));
//	rvecs.push_back(cv::Point3d(-3.2672263761474576e+00, -6.1639281152975317e-02, -3.3270461545651420e-01));
//	tvecs.push_back(cv::Point3d(-4.1536179216061508e-01, 5.9554349411597853e-02, 2.5452326723970713e+00));
//	tvecs.push_back(cv::Point3d(-2.0051985299720090e-01, 5.3624216915851008e-02, 2.5375654403779166e+00));
//	for (int i = 0; i < tvecs.size(); i++)
//	{
//		cv::Point3d tVec = tvecs[i];
//		cv::Point3d resVec;
//
//		resVec.x = affTransform.at<double>(0, 0) * tVec.x + affTransform.at<double>(0, 1) * tVec.y + affTransform.at<double>(0, 2)*tVec.z + affTransform.at<double>(0, 3) * 1;
//		resVec.y = affTransform.at<double>(1, 0) * tVec.x + affTransform.at<double>(1, 1) * tVec.y + affTransform.at<double>(1, 2)*tVec.z + affTransform.at<double>(1, 3) * 1;
//		resVec.z = affTransform.at<double>(2, 0) * tVec.x + affTransform.at<double>(2, 1) * tVec.y + affTransform.at<double>(2, 2)*tVec.z + affTransform.at<double>(2, 3) * 1;
//		
//		printf("Groundtruth:  (%f, %f, %f) \n", cP[i].x, cP[i].y, cP[i].z);
//		printf("Transformed Vector: (%f, %f, %f) \n", resVec.x, resVec.y, resVec.z);
//		printf("EROOR: %f, %f, %f \n \n", std::abs(resVec.x - cP[i].x), std::abs(resVec.y - cP[i].y), std::abs(resVec.z - cP[i].z));
//
//	}
//
//
//}
int Calibration::runPoseEstimation(uEye_input* uei)
{
	int ret = pe->generateCamMatAndDistMat(uei);
	//pe->loadSavedDistCoeff();
	UndistortRectifyMaps = pe->generateUndistortRectifyMap();
	pc->distCoeffs = pe->getDistCoeffs();
	pc->camMatrix = pe->getCameraMat();
	return ret;
}



int Calibration::catchPlaneMarker(cv::Mat frame)
{
	int ret = pc->detectAruco(frame);
	return ret;
} 

int Calibration::generatePlaneCalib()
{
	int ret = pc->computePlaneCalibration();
	int rep = pc->computeAffineTransformation();
	pcd = pc->getPlaneCalibData();
	return ret;
}



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



