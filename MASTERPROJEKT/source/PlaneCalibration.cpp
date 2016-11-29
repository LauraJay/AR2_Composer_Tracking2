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
	}
	return 0;
}

PlaneCalibration::PlaneCalibration(){
	initAruco();
}

std::vector<cv::Point2f> PlaneCalibration::getPlaneCorners() {
	return std::vector<cv::Point2f>{upperLeft, upperRight, lowerRight, lowerLeft};
}

int PlaneCalibration::runPlaneCalibration(cv::Mat frame){
	return runMarkerDetection(frame);
}
