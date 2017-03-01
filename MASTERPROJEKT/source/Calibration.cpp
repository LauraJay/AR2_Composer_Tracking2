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

void Calibration::testNewCalibWithPositions() {
	cv::Mat P;
	std::vector< cv::Point3f> cP;
	cv::FileStorage fs("P.yml", cv::FileStorage::READ);
	fs["P"] >> P;
	std::vector< cv::Vec3d > rvecs, tvecs;
	std::vector< cv::Point2d >	calibMarkerPos2D;
	std::vector<std::vector<cv::Point2d>> corners;
	cv::FileStorage fsd("correspondingPoints.yml", cv::FileStorage::READ);
	cv::Point3d temp;
	fs["Marker Corners"] >> temp;
	for (int i = 1; i <= 2; i++)
	{
	std::string file = "Zusammenfassung" + std::to_string(i);
	file += ".yml";
	cv::FileStorage fs(file, cv::FileStorage::READ);
	std::string tvec = "Tvecs"+ std::to_string(i);
	std::vector< cv::Vec3d > temp;
	fs[tvec] >> temp;
	tvecs.push_back(temp[0]);
	std::string rvec = "Rvecs" + std::to_string(i);
	fs[rvec] >> temp;
	rvecs.push_back(temp[0]);
	std::string mCorn = "Marker Corners" + std::to_string(i);
	std::vector<std::vector<cv::Point2d>> temp2;
	fs[mCorn] >> temp2;
	corners.push_back(temp2[0]);
	std::string mCen = "Marker center" + std::to_string(i);
	std::vector< cv::Point2d > temp3;
	fs["Marker center"] >> temp3;
	calibMarkerPos2D.push_back(temp3[0]);
	}

	/*cv::Mat testImg = cv::Mat::zeros(1280, 1024, CV_8UC3);

	for (int i = 0; i < tvecs.size(); i++)
	{

		cv::Point2d testPixCoords;
		cv::Mat tvecMat = cv::Mat(1, 3, CV_64F);
		tvecMat.at<double>(0, 0) = tvecs[i].x;
		tvecMat.at<double>(0, 1) = tvecs[i].y;
		tvecMat.at<double>(0, 2) = tvecs[i].z;
		tvecMat = tvecMat * P.inv();
		testPixCoords.x = tvecMat.at<double>(0, 0);
		testPixCoords.y = tvecMat.at<double>(0, 1);

		cv::circle(testImg, testPixCoords, 4, cv::Scalar(0, 255, 0));
		cv::circle(testImg, calibMarkerPos2D[i], 4, cv::Scalar(255, 0, 0));

	}*/

	//cv::namedWindow("debug", cv::WINDOW_KEEPRATIO);
	//cv::imshow("debug", testImg);
	//cv::waitKey(1);


}
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



