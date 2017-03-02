#include "PlaneCalibration.h"

void PlaneCalibration::initAruco() {
	dictionaryId = cv::aruco::DICT_4X4_50;
	markerLength = 0.133825; // size of outprinted Marker
	detectorParams = cv::aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers
	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
}

PlaneCalibration::planeCalibData PlaneCalibration::readPlaneCalibrationFile() {
	
	calibFile.open("TrackingPlaneCalibration.txt", std::ios::in);
	if (calibFile.is_open()) {
		std::string lineX;
		std::string lineY;
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		pcd.upperCorner = cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str()));
		getline(calibFile, lineX);
		getline(calibFile, lineY);
	pcd.lowerCorner = cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str()));
	}
	return pcd;
}

int PlaneCalibration::detectAruco(cv::Mat frame) {
	//cv::FileStorage fs("CorrespondingPoints.yml", cv::FileStorage::WRITE);
	//if (!fs.isOpened())
	//	return false;

	std::vector<int> arucoIds;
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> rejected;
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	if (arucoIds.size() < 1) {
		printf("No markers found. Please ensure that the marker of the controller is located in the image.\n");
		return -1;
	}
	// detect markers and estimate pose
	if (arucoIds.size() > 0)
		cv::aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);
	AllTVecs.push_back(tvecs[0]);
	AllRVecs.push_back(rvecs[0]);

	bool foundID = false;
	// draw results
		//if (arucoIds.size() > 0) cv::aruco::drawDetectedMarkers(frame, corners);
	for (int i = 0; i < arucoIds.size(); i++) {
		int ID = arucoIds.at(i);
		if (ID == 49) {
            foundID = true;
            cv::Point2f baricenter(0, 0);
		    std::vector<cv::Point2f> p = corners.at(i);
            for (int i = 0; i < p.size(); i++) {
                baricenter += p[i];
            }
            baricenter.x /= p.size();
            baricenter.y /= p.size();
            markerPositions.push_back(baricenter);

			/*fs << "Marker center" << markerPositions;
			fs << "Marker corner" << corners;
			fs << "Tvecs" << tvecs;
			fs << "Rvecs" << rvecs;*/

        break;	// Corner 1 is upper right corner of marker, which
		}



	}

	if (!foundID) {
		printf(" Calibration Marker not found. Please ensure that the marker of the controller is located in the image.\n");
		
	}
	return markerPositions.size();
}

 PlaneCalibration::planeCalibData PlaneCalibration::getPlaneCalibData()
{
	return pcd;
}

PlaneCalibration::PlaneCalibration() {
	initAruco();

}

void PlaneCalibration::debugCamMatImg() {

	cv::Mat testImg = cv::Mat::zeros(1280, 1024, CV_8UC3);

		std::vector< cv::Point3f > axisPoints;
		axisPoints.push_back(cv::Point3f(0, 0, 0));
	for (int i = 0; i < AllTVecs.size(); i++)
	{
		std::vector< cv::Point2f > imagePoints;
		std::vector<cv::Vec3d> Vec3rves, Vec3tves;
		cv::Vec3d tempr = AllRVecs[i];
		cv::Vec3d tempt = AllTVecs[i];
		Vec3rves.push_back(tempr);
		Vec3tves.push_back(tempt);
		projectPoints(axisPoints, Vec3rves[i], Vec3tves[i], camMatrix, distCoeffs, imagePoints);
		cv::circle(testImg, imagePoints[0], 5, cv::Scalar(0, 255, 0));
		cv::circle(testImg, markerPositions[i], 3, cv::Scalar(255, 0, 0));

	}

	cv::namedWindow("debug2", cv::WINDOW_KEEPRATIO);
	cv::imshow("debug2", testImg);
	cv::waitKey(1);
}

void PlaneCalibration::printControllerPosError()
{
	printf("ControllerPos Error after Plane Calibration: \n");
	for (int i = 0; i < AllTVecs.size(); i++)
			{
				cv::Point3d tVec = AllTVecs[i];
				cv::Point3d resVec;
		
				resVec.x = affTransform.at<double>(0, 0) * tVec.x + affTransform.at<double>(0, 1) * tVec.y + affTransform.at<double>(0, 2)*tVec.z + affTransform.at<double>(0, 3) * 1;
				resVec.y = affTransform.at<double>(1, 0) * tVec.x + affTransform.at<double>(1, 1) * tVec.y + affTransform.at<double>(1, 2)*tVec.z + affTransform.at<double>(1, 3) * 1;
				resVec.z = affTransform.at<double>(2, 0) * tVec.x + affTransform.at<double>(2, 1) * tVec.y + affTransform.at<double>(2, 2)*tVec.z + affTransform.at<double>(2, 3) * 1;
				
				printf("Groundtruth of No %i:  (%f, %f, %f) \n", i,AllControllerPositions[i].x, AllControllerPositions[i].y, AllControllerPositions[i].z);
				printf("Transformed Vectorof No %i:  (%f, %f, %f) \n",i, resVec.x, resVec.y, resVec.z);
				printf("EROOR of No %i: : %f, %f, %f \n \n",i, std::abs(resVec.x - AllControllerPositions[i].x), std::abs(resVec.y - AllControllerPositions[i].y), std::abs(resVec.z - AllControllerPositions[i].z));
		
			}



}

int PlaneCalibration::getSizeOfMarkerPos() {
	return markerPositions.size();
}

int PlaneCalibration::computePlaneCalibration() {
	
	if (markerPositions.size() >= 2) {
	if (markerPositions[0].y < markerPositions[1].y) {
		pcd.upperCorner = markerPositions[0];
		pcd.lowerCorner = markerPositions[1];
	}
	else {
		pcd.upperCorner = markerPositions[1];
		pcd.lowerCorner = markerPositions[0];
	}
	pcd.size = cv::Size(pcd.upperCorner.x - pcd.lowerCorner.x, pcd.lowerCorner.y - pcd.upperCorner.y);
		calibFile.open("TrackingPlaneCalibration.txt", std::ios::out);
		if (calibFile.is_open()) {
			calibFile << pcd.upperCorner.x << "\n";
			calibFile << pcd.upperCorner.y << "\n";
			calibFile << pcd.lowerCorner.x << "\n";
			calibFile << pcd.lowerCorner.y << "\n";
			calibFile << pcd.size.width << "\n";
			calibFile << pcd.size.height << "\n";
			calibFile.close();
		}
		else {
			printf("Error writing calibration to file!\n");
			return -1;
		}
	}
	else return -1;
	return 1;
}

PlaneCalibration::planeCalibData PlaneCalibration::loadImagePlane()
{
	std::fstream calibFile;
	PlaneCalibration::planeCalibData pcd;
	calibFile.open("TrackingPlaneCalibration.txt", std::ios::in);
	if (calibFile.is_open()) {
		std::string lineX;
		std::string lineY;
		std::string width;
		std::string height;
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		pcd.upperCorner = (cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str())));
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		pcd.lowerCorner = (cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str())));
		getline(calibFile, width);
		getline(calibFile, height);
		pcd.size = (cv::Size(::atof(width.c_str()), ::atof(height.c_str())));
		calibFile.close();
	}
		return pcd;
}

int PlaneCalibration::computeAffineTransformation() {


	std::vector< uchar >  inliers;
	cv::estimateAffine3D(AllTVecs, AllControllerPositions, affTransform, inliers);
	std::vector< uchar >  inliers2;
	cv::estimateAffine3D(AllControllerPositions, AllTVecs, invAffTransform, inliers2);

	return 1;
}
bool PlaneCalibration::saveAffineTransform() {
	cv::FileStorage fs("AffineTransform.yml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;
	
	fs << "affineTransform" << affTransform;
	return true;
}
bool PlaneCalibration::loadAffineTransform() {
	
	cv::FileStorage fs("AffineTransform.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["affineTransform"] >> affTransform;
	true;
}