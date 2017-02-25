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
			markerSize.push_back(computeSizeDifferenceOfAruco(corners.at(i)));

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

void PlaneCalibration::initDebugValues() {

	markerPositions.push_back(cv::Point2f(128.142578, 759.935303));
	markerPositions.push_back(cv::Point2f(1226.188232, 69.112511));
	markerPositions.push_back(cv::Point2f(1181.037354, 790.379150));

	/*controllerPositions.push_back(cv::Point3f(-0.1192911, 0.6097684, -0.2195672));
	controllerPositions.push_back(cv::Point3f(0.6733468, 0.6219791, 0.2867339));
	controllerPositions.push_back(cv::Point3f(0.7260225, 0.6135228, -0.1738673));*/
	controllerPositions.push_back(cv::Point3f(-0.1192911, -0.2195672, 0.6097684));
	controllerPositions.push_back(cv::Point3f(0.6733468, 0.2867339, 0.6219791));
	controllerPositions.push_back(cv::Point3f(0.7260225, -0.1738673, 0.6135228));
}

std::vector<cv::Point3f> transform(cv::Mat screen2Cam, std::vector<cv::Point2f> markerPos) {
	std::vector<cv::Point3f> temp;
	//temp.at(0) = screen2Cam.mul(marker)


	return temp;
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
float PlaneCalibration::computeSizeDifferenceOfAruco(std::vector<cv::Point2f> corners) {
	cv::Point2f b1 = corners[1] - corners[0];
	float b1float = std::sqrt(b1.x*b1.x + b1.y * b1.y);
	cv::Point2f a1 = corners[2] - corners[1];
	float a1float = std::sqrt(a1.x*a1.x + a1.y * a1.y);
	cv::Point2f c1 = corners[0] - corners[2];
	float c1float = std::sqrt(c1.x*c1.x + c1.y * c1.y);
	float alpha1 = std::acos(((a1.dot(b1)))/ (std::sqrt(a1.x*a1.x + a1.y*a1.y)*std::sqrt(b1.x*b1.x + b1.y*b1.y)));
	float h = c1float * std::sinf(alpha1);
	float A = 0.5 * b1float*h;
	
	cv::Point2f b2 = corners[2] - corners[1];
	float b2float = std::sqrt(b2.x*b2.x + b2.y * b2.y);
	cv::Point2f a2 = corners[3] - corners[2];
	float a2float = std::sqrt(a2.x*a2.x + a2.y * a2.y);
	cv::Point2f c2 = corners[1] - corners[3];
	float c2float = std::sqrt(c2.x*c2.x + c2.y * c2.y);
	float alpha2 = std::acos(((a2.dot(b2))) / (std::sqrt(a2.x*a2.x + a2.y*a2.y)*std::sqrt(b2.x*b2.x + b2.y*b2.y)));
	float h2 = c2float * std::sinf(alpha2);
	 A += 0.5 * b2float*h2;


	printf("Fläche: %f \n",A);
	return A;
}