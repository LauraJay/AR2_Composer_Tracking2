#include "CoordsTransformation2Unity.h"

int CoordsTransformtion2Untiy::computeTransformation2Unity(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec)
{
	for (int i = 0; i < takenIdVec.size(); i++)
	{
		int id = takenIdVec[i];
		Marker* m = allMarkers[id];
		std::vector<cv::Point3d> tVecs;
		cv::Point3d estiCenter;
		tVecs = m->getTvecs();
		cv::Point3d tVec = tVecs[0];
		estiCenter.x = affineTransform.at<double>(0, 0) * tVec.x + affineTransform.at<double>(0, 1) * tVec.y + affineTransform.at<double>(0, 2)*tVec.z + affineTransform.at<double>(0, 3) * 1;
		estiCenter.y = affineTransform.at<double>(1, 0) * tVec.x + affineTransform.at<double>(1, 1) * tVec.y + affineTransform.at<double>(1, 2)*tVec.z + affineTransform.at<double>(1, 3) * 1;
		estiCenter.z = affineTransform.at<double>(2, 0) * tVec.x + affineTransform.at<double>(2, 1) * tVec.y + affineTransform.at<double>(2, 2)*tVec.z + affineTransform.at<double>(2, 3) * 1;
		m->setEstimatedCenter(estiCenter);
	}
	return 1;
}

void CoordsTransformtion2Untiy::debugEstiCenterImg(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec) {

	cv::Mat testImg = cv::Mat::zeros(1280, 1024, CV_8UC3);

	for (int i = 0; i < takenIdVec.size(); i++)
	{
		int id = takenIdVec[i];
		Marker* m = allMarkers[id];
		std::vector<cv::Point3d> tVecs;
		cv::Point3d estiCenter= m->getEstimatedCenter();
		cv::Point3d tVec ;
	
		tVec.x = invAffTransform.at<double>(0, 0) * estiCenter.x + invAffTransform.at<double>(0, 1) * estiCenter.y + invAffTransform.at<double>(0, 2)*estiCenter.z + invAffTransform.at<double>(0, 3) * 1;
		tVec.y = invAffTransform.at<double>(1, 0) * estiCenter.x + invAffTransform.at<double>(1, 1) * estiCenter.y + invAffTransform.at<double>(1, 2)*estiCenter.z + invAffTransform.at<double>(1, 3) * 1;
		tVec.z = invAffTransform.at<double>(2, 0) * estiCenter.x + invAffTransform.at<double>(2, 1) * estiCenter.y + invAffTransform.at<double>(2, 2)*estiCenter.z + invAffTransform.at<double>(2, 3) * 1;

		std::vector< cv::Point3f > axisPoints;
		axisPoints.push_back(cv::Point3f(0, 0, 0));
		std::vector< cv::Point2f > imagePoints;
		std::vector<cv::Vec3d> Vec3rves, Vec3tves;
	
			cv::Vec3d tempr =m->getRvecs()[0];
			cv::Vec3d tempt = tVec;
			Vec3rves.push_back(tempr);
			Vec3tves.push_back(tempt);

		

		projectPoints(axisPoints, Vec3rves[i], Vec3tves[i], camMatrix, distCoeffs, imagePoints);
		cv::circle(testImg, imagePoints[0], 5, cv::Scalar(0, 255, 0));
		cv::circle(testImg, m->getCenter(), 3, cv::Scalar(255, 0, 0));

	}

	cv::namedWindow("debug3", cv::WINDOW_KEEPRATIO);
	cv::imshow("debug3", testImg);
	cv::waitKey(1);
}




