#include "PoseEstimation.h"
#include <iostream>
#include <fstream>

PoseEstimation::PoseEstimation()
{
}

cv::Mat PoseEstimation::getCameraMat()
{
	return cameraMatrix;
}

cv::Mat PoseEstimation::getDistCoeffs()
{
	return distCoeffs;
}

std::vector<cv::Mat> PoseEstimation::generateUndistortRectifyMap()
{
	cv::Mat  map1, map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, size, 1, size),size, CV_16SC2, map1, map2);

	cv::Mat dst, image;
	image = cv::imread("Checkerboard.jpg", CV_LOAD_IMAGE_COLOR);
	remap(image, dst, map1, map2, cv::INTER_LINEAR);
	cv::imshow("undistortedImg", dst);
	cv::waitKey(1);
	cv::imwrite("UeyeDistCoeffs.jpg", dst);
	std::vector<cv::Mat> maps;
	maps.push_back(map1);
	maps.push_back(map2);
	bool saveOK =saveMaps(maps);
	if (!saveOK)printf("Saving UndistortRectifyMap failed.");
	return maps;
}


int PoseEstimation::generateCamMatAndDistMat(uEye_input * uei)
{
	printf("Calibration using a ChArUco board\n");
	printf("  To capture a frame for calibration, press 'c',\n");
	printf("  To finish capturing, press 'ESC' key and calibration starts.\n");

	cv::Mat frame;
	int squaresX = 8;
	int squaresY = 8;
	float squareLength = 0.034;
	float markerLength = 0.023;
	int dictionaryId = 16;
	std::string outputFile = "CameraMatrix.yml";
	bool fixAspectRatio = false;
	bool dozeroTangent = false;
	bool doPrincipalPoint = false;
	bool showChessboardCorners = false;

	int calibrationFlags = 0;
	float aspectRatio = 1;
	if (fixAspectRatio) {
		calibrationFlags |= cv::CALIB_FIX_ASPECT_RATIO;
		aspectRatio = 1;
	}
	if (dozeroTangent) calibrationFlags |= cv::CALIB_ZERO_TANGENT_DIST;
	if (doPrincipalPoint) calibrationFlags |= cv::CALIB_FIX_PRINCIPAL_POINT;

	cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();

	bool refindStrategy = false;

	frame=uei->getCapturedFrame();
	cv::Ptr<cv::aruco::Dictionary> dictionary =
		cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

	// create charuco board object
	cv::Ptr<cv::aruco::CharucoBoard> charucoboard =
		cv::aruco::CharucoBoard::create(squaresX, squaresY, squareLength, markerLength, dictionary);
	cv::Ptr<cv::aruco::Board> board = charucoboard.staticCast<cv::aruco::Board>();

	// collect data from each frame
	std::vector< std::vector< std::vector< cv::Point2f > > > allCorners;
	std::vector< std::vector< int > > allIds;
	std::vector< cv::Mat > allImgs;


	while (!frame.empty()) {
		cv::Mat imageCopy;
		frame = uei->getCapturedFrame();

		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejected;

		// detect markers
		cv::aruco::detectMarkers(frame, dictionary, corners, ids, detectorParams, rejected);

		// refind strategy to detect more markers
		if (refindStrategy) cv::aruco::refineDetectedMarkers(frame, board, corners, ids, rejected);

		// interpolate charuco corners
		cv::Mat currentCharucoCorners, currentCharucoIds;
		if (ids.size() > 0)
			cv::aruco::interpolateCornersCharuco(corners, ids, frame, charucoboard, currentCharucoCorners,
				currentCharucoIds);

		// draw results
		frame.copyTo(imageCopy);
		if (ids.size() > 0) cv::aruco::drawDetectedMarkers(imageCopy, corners);

		if (currentCharucoCorners.total() > 0)
			cv::aruco::drawDetectedCornersCharuco(imageCopy, currentCharucoCorners, currentCharucoIds);

		putText(imageCopy, "Press 'c' to add current frame. 'ESC' to finish and calibrate",
			cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
		
		imshow("out", imageCopy);
		char key = (char)cv::waitKey(1);
		if (key == 27) break;
		if (key == 'c' && ids.size() > 0) {
			std::cout << "Frame captured" << std::endl;
			allCorners.push_back(corners);
			allIds.push_back(ids);
			allImgs.push_back(frame);
			size = frame.size();
		}
	}
	printf("Starting CamParam computation.\n");
	if (allIds.size() < 1) {
		std::cerr << "Not enough captures for calibration\n" << std::endl;
		return -1;
	}


	std::vector< cv::Mat > rvecs, tvecs;
	double repError;

	if (calibrationFlags & cv::CALIB_FIX_ASPECT_RATIO) {
		cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		cameraMatrix.at< double >(0, 0) = aspectRatio;
	}

	// prepare data for calibration
	std::vector< std::vector< cv::Point2f > > allCornersConcatenated;
	std::vector< int > allIdsConcatenated;
	std::vector< int > markerCounterPerFrame;
	markerCounterPerFrame.reserve(allCorners.size());
	for (unsigned int i = 0; i < allCorners.size(); i++) {
		markerCounterPerFrame.push_back((int)allCorners[i].size());
		for (unsigned int j = 0; j < allCorners[i].size(); j++) {
			allCornersConcatenated.push_back(allCorners[i][j]);
			allIdsConcatenated.push_back(allIds[i][j]);
		}
	}
	// calibrate camera using cv::aruco markers
	double arucoRepErr;
	arucoRepErr = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
		markerCounterPerFrame, board, size, cameraMatrix,
		distCoeffs, cv::noArray(), cv::noArray(), calibrationFlags);

	// prepare data for charuco calibration
	int nFrames = (int)allCorners.size();
	std::vector< cv::Mat > allCharucoCorners;
	std::vector< cv::Mat > allCharucoIds;
	std::vector< cv::Mat > filteredImages;
	allCharucoCorners.reserve(nFrames);
	allCharucoIds.reserve(nFrames);

	for (int i = 0; i < nFrames; i++) {
		// interpolate using camera parameters
		cv::Mat currentCharucoCorners, currentCharucoIds;
		cv::aruco::interpolateCornersCharuco(allCorners[i], allIds[i], allImgs[i], charucoboard,
			currentCharucoCorners, currentCharucoIds, cameraMatrix,
			distCoeffs);

		allCharucoCorners.push_back(currentCharucoCorners);
		allCharucoIds.push_back(currentCharucoIds);
		filteredImages.push_back(allImgs[i]);
	}

	if (allCharucoCorners.size() < 4) {
		std::cerr << "Not enough corners for calibration" << std::endl;
		return -1;
	}

	// calibrate camera using charuco
	repError =
		cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, size,
			cameraMatrix, distCoeffs, cv::noArray(), cv::noArray(), calibrationFlags);

	printf("rep Error: %f \n",repError);
	bool saveOk = saveCameraParams(outputFile, size, aspectRatio, calibrationFlags,
		cameraMatrix, distCoeffs, repError);
	if (!saveOk) {
		std::cerr << "Cannot save output file" << std::endl;
		return -1;
	}
	std::cout << "Camera Calibration saved to " << outputFile << std::endl;


	return 1;
}


bool PoseEstimation::loadSavedDistCoeff()
{
	cv::FileStorage fs("DistCoMat.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs [ "distortion_coefficients"] >> PoseEstimation::distCoeffs;
	return true;
}




bool PoseEstimation::saveMaps(const std::vector <cv::Mat> maps) {
	cv::FileStorage fs("Maps.yml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;
	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);
	fs << "calibration_time" << buf;
	fs << "Map1" << maps[0];
	fs << "Map2" << maps[1];
	return true;
}

std::vector <cv::Mat> PoseEstimation::loadUndistortRectifyMaps() {
	std::vector <cv::Mat> maps;
	cv::Mat mat1, mat2;
	cv::FileStorage fs("Maps.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return maps;
	fs["Map1"] >> mat1;
	maps.push_back(mat1);
	fs["Map2"] >> mat2;
	maps.push_back(mat2);
	
	return maps;
}




bool PoseEstimation::saveCameraParams(const std::string &filename, cv::Size imageSize, float aspectRatio, int flags,
	const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, double totalAvgErr) {
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;

	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);

	fs << "calibration_time" << buf;

	fs << "image_width" << imageSize.width;
	fs << "image_height" << imageSize.height;

	if (flags & cv::CALIB_FIX_ASPECT_RATIO) fs << "aspectRatio" << aspectRatio;

	if (flags != 0) {
		sprintf(buf, "flags: %s%s%s%s",
			flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
			flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
			flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
			flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
	}

	fs << "flags" << flags;
	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	fs << "avg_reprojection_error" << totalAvgErr;
	printf("CamParams Saved\n");
	return true;
}

bool PoseEstimation::loadCameraMat() {
	cv::FileStorage fs("CameraMatrix.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["camera_matrix"] >> cameraMatrix;
	fs ["image_width"] >> size.width;
	fs [ "image_height" ]>> size.height;
	return true;
}
