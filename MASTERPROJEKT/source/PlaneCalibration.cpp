#include "PlaneCalibration.h"

void PlaneCalibration::initAruco() {
	dictionaryId = cv::aruco::DICT_4X4_50;
	markerLength = 0.019; // size of outprinted Marker
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
	std::vector<int> arucoIds;
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> rejected;
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	if (arucoIds.size() < 1) {
		printf("Please ensure that the marker of the controller is located in the image");
		return -1;
	}
	bool foundID = false;
	// draw results
		//if (arucoIds.size() > 0) cv::aruco::drawDetectedMarkers(frame, corners);
	for (int i = 0; i < arucoIds.size(); i++) {

		int ID = arucoIds.at(i);
		if (ID == 10) {
			std::vector<cv::Point2f> p = corners.at(i);
			float centerX, centerY;
			cv::Point2f d1 = (p[2] - p[0]);
			cv::Point2f c = (d1)/2+p[0];
			circle(frame, c, 1, cv::Scalar(0, 225, 255));
			markerPositions.push_back(c); 
			foundID = true;
			break;	// Corner 1 is upper right corner of marker, which
		}
	}

	if (!foundID) {
		printf("Please ensure that the marker of the controller is located in the image");
		return -1;
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

int PlaneCalibration::getSizeOfMarkerPos() {
	return markerPositions.size();
}

int PlaneCalibration::computePlaneCalibration() {
	
	if (markerPositions.size() == 2) {
	if (markerPositions[0].y < markerPositions[1].y) {
		pcd.upperCorner = markerPositions[0];
		pcd.lowerCorner = markerPositions[1];
	}
	else {
		pcd.upperCorner = markerPositions[1];
		pcd.lowerCorner = markerPositions[0];
	}
	pcd.size = cv::Size(std::abs(pcd.upperCorner.x - pcd.lowerCorner.x), pcd.lowerCorner.y - pcd.upperCorner.y);
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
			printf("Error writing calibration to file!");
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