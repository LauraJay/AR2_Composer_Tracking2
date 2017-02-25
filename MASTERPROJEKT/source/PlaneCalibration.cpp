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
	std::vector<int> arucoIds;
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> rejected;
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	if (arucoIds.size() < 1) {
		printf("No markers found. Please ensure that the marker of the controller is located in the image.\n");
		return -1;
	}
	//std::vector< cv::Vec3d > rvecs, tvecs;	// detect markers and estimate pose
	//	if (arucoIds.size() > 0)
	//		cv::aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);

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

int PlaneCalibration::getSizeOfMarkerPos() {
	return markerPositions.size();
}

int PlaneCalibration::computePlaneCalibration() {
	
	if (markerPositions.size() == 3) {
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
