#include "PlaneCalibration.h"

void PlaneCalibration::initAruco() {
	dictionaryId = cv::aruco::DICT_ARUCO_ORIGINAL;
	markerLength = 0.09; // size of outprinted Marker
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
	for (int i = 0; i < arucoIds.size(); i++) {

		int ID = arucoIds.at(i);
		if (ID == 800) {
			markerPositions.push_back(corners.at(i).at(1)); 
			break;	// Corner 1 is upper right corner of marker, which
		}
	}

	if (arucoIds.size() < 1) {
		printf("Please ensure that the marker of the controller is located in the image");
		return -1;
	}
	return 1;
}

 PlaneCalibration::planeCalibData PlaneCalibration::getPlaneCalibData()
{
	return pcd;
}

PlaneCalibration::PlaneCalibration() {
	initAruco();

}

int PlaneCalibration::runPlaneCalibration(uEye_input* uei) {
	cv::Mat frame;
	int ret = 0;
	std::cout << "Press c to capture" << std::endl;
	frame = uei->getCapturedFrame();
	while (!frame.empty()) {
		frame = uei->getCapturedFrame();
		cv::imshow("out", frame);
		char key = (char)cv::waitKey(10);
		if (key == 'c') {
			int ret = detectAruco(frame);
			std::cout << "Frame captured" << std::endl;
		}
		if (markerPositions.size() == 2) break;
	}
	if (markerPositions.size() == 2) {
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
			calibFile.close();
		}
		else {
			printf("Error writing calibration to file!");
			return 0;
		}
	}
	else return 0;
	return 1;
}
