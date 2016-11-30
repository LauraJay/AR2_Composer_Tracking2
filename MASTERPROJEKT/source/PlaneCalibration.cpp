#include "PlaneCalibration.h"

void PlaneCalibration::initAruco() {
	dictionaryId = cv::aruco::DICT_ARUCO_ORIGINAL;
	markerLength = 0.09; // size of outprinted Marker
	detectorParams = cv::aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers
	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
}

int PlaneCalibration::runMarkerDetection(cv::Mat &frame){
	arucoIds.clear();
	corners.clear();
	rejected.clear();
	return detectAruco(frame);
}

int PlaneCalibration::detectAruco(cv::Mat frame) {
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	for (int i = 0; i < arucoIds.size(); i++) {
		int ID = arucoIds.at(i);
		switch (ID) {
		case 200:	upperRight = corners.at(i).at(1); break;	// Corner 1 is upper right corner of marker, which
		case 400:	upperLeft = corners.at(i).at(1); break;		// is oriented outwards on all our wood markers.
		case 600:	lowerLeft = corners.at(i).at(1); break;
		case 800:	lowerRight = corners.at(i).at(1); break;
		default: return -1;
		}
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

	}
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
