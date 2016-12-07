#include "PlaneCalibration.h"

void PlaneCalibration::initAruco() {
	dictionaryId = cv::aruco::DICT_ARUCO_ORIGINAL;
	markerLength = 0.09; // size of outprinted Marker
	detectorParams = cv::aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers
	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
}

int PlaneCalibration::runMarkerDetection(cv::Mat &frame){
	calibFile.open("TrackingPlaneCalibration.txt", std::ios::in);
	if (calibFile.is_open()) {
		std::string lineX;
		std::string lineY;
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		upperRight = cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str()));
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		upperLeft = cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str()));
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		lowerLeft = cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str()));
		getline(calibFile, lineX);
		getline(calibFile, lineY);
		lowerRight = cv::Point2f(::atof(lineX.c_str()), ::atof(lineY.c_str()));
		calibFile.close();
		return 0;
	}
	arucoIds.clear();
	corners.clear();
	rejected.clear();
	return detectAruco(frame);
}

int PlaneCalibration::detectAruco(cv::Mat frame) {
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	if (arucoIds.size() != 4)
		return -1;
	for (int i = 0; i < 4; i++) {
		int ID = arucoIds.at(i);
		switch (ID) {
		case 200:	upperRight = corners.at(i).at(1); break;	// Corner 1 is upper right corner of marker, which
		case 400:	upperLeft = corners.at(i).at(1); break;		// is oriented outwards on all our wood markers.
		case 600:	lowerLeft = corners.at(i).at(1); break;
		case 800:	lowerRight = corners.at(i).at(1); break;
		default: printf("Error detecting calibration marker!"); return -1;
		}
	}
	calibFile.open("TrackingPlaneCalibration.txt", std::ios::out);
	if (calibFile.is_open()) {
		calibFile << upperRight.x << "\n";
		calibFile << upperRight.y << "\n";
		calibFile << upperLeft.x << "\n";
		calibFile << upperLeft.y << "\n";
		calibFile << lowerLeft.x << "\n";
		calibFile << lowerLeft.y << "\n";
		calibFile << lowerRight.x << "\n";
		calibFile << lowerRight.y << "\n";
		calibFile.close();
	}
	else
		printf("Error writing calibration to file!");

	// Ensure that plane is a rectangle by choosing the x and y 
	// coordinates closest to the center for each respective side
	cv::Point2f center = cv::Point2f(
		upperRight.x * 0.25f +
		upperLeft.x * 0.25f +
		lowerRight.x * 0.25f +
		lowerLeft.x * 0.25f,
		upperRight.y * 0.25f +
		upperLeft.y * 0.25f +
		lowerRight.y * 0.25f +
		lowerLeft.y * 0.25f
	);
	if (upperLeft.x - center.x <= lowerLeft.x - center.x)
		lowerLeft.x = upperLeft.x;
	else
		upperLeft.x = lowerLeft.x;

	if (upperRight.x - center.x <= lowerRight.x - center.x)
		lowerRight.x = upperRight.x;
	else
		upperRight.x = lowerRight.x;

	if (upperLeft.y - center.y <= upperRight.y - center.y)
		upperRight.y = upperLeft.y;
	else
		upperLeft.y = upperRight.y;

	if (lowerLeft.y - center.y <= lowerRight.y - center.y)
		lowerRight.y = lowerLeft.y;
	else
		lowerLeft.y = lowerRight.y;

	return 0;
}

PlaneCalibration::PlaneCalibration(){
	initAruco();
}

PlaneCalibration::planeCalibData PlaneCalibration::getPlaneCalibData() {
	planeCalibData pcd;
	pcd.upperLeftCorner = upperLeft;
	pcd.size = cv::Size(upperRight.x - upperLeft.x, upperLeft.y - lowerLeft.y);
	return pcd;
}

int PlaneCalibration::runPlaneCalibration(cv::Mat frame){
	return runMarkerDetection(frame);
}
