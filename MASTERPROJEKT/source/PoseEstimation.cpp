#include "PoseEstimation.h"

int PoseEstimation::runPoseEstimation(uEye_input* uei)
{
	
	//TODO fill with correct data
	int squaresX = 8;
	int squaresY =8;
	float squareLength = 0.034;
	float markerLength = 0.023;
	int dictionaryId = cv::aruco::DICT_ARUCO_ORIGINAL;
	std::string outputFile = "CameraCalib.txt";

	bool showChessboardCorners = false;

	int calibrationFlags = 0;
	float aspectRatio = 1;
	
	if (false) calibrationFlags |= cv::CALIB_ZERO_TANGENT_DIST;
	if (false) calibrationFlags |= cv::CALIB_FIX_PRINCIPAL_POINT;

	cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();
	bool refindStrategy = false;
	
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
	cv::Size imgSize;
	
	while (true) {
		cv::Mat image, imageCopy;
		image = uei->getCapturedFrame();
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejected;

		// detect markers
		cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

		// refind strategy to detect more markers
		if (refindStrategy) cv::aruco::refineDetectedMarkers(image, board, corners, ids, rejected);

		// interpolate charuco corners
		cv::Mat currentCharucoCorners, currentCharucoIds;
		if (ids.size() > 0)
			cv::aruco::interpolateCornersCharuco(corners, ids, image, charucoboard, currentCharucoCorners,
				currentCharucoIds);

		// draw results
		image.copyTo(imageCopy);
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
			allImgs.push_back(image);
			imgSize = image.size();
		}
	}

	if (allIds.size() < 1) {
		std::cerr << "Not enough captures for calibration" << std::endl;
	}

	cv::Mat cameraMatrix, distCoeffs;
	std::vector<cv::Mat > rvecs, tvecs;
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

	// calibrate camera using aruco markers
	double arucoRepErr;
	arucoRepErr = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
		markerCounterPerFrame, board, imgSize, cameraMatrix,
		distCoeffs, rvecs, tvecs, calibrationFlags);

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
	}

	// calibrate camera using charuco
	repError =
		cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, imgSize,
			cameraMatrix, distCoeffs, rvecs, tvecs, calibrationFlags);

	//bool saveOk = saveCameraParams(outputFile, imgSize, aspectRatio, calibrationFlags,
	//	cameraMatrix, distCoeffs, rvecs, tvecs, repError);
	//if (!saveOk) {
	//	std::cerr << "Cannot save output file" << std::endl;
	//	return 0;
	//}

	std::cout << "Rep Error: " << repError << std::endl;
	std::cout << "Rep Error Aruco: " << arucoRepErr << std::endl;
	std::cout << "Calibration saved to " << outputFile << std::endl;

	// show interpolated charuco corners for debugging
	if (showChessboardCorners) {
		for (unsigned int frame = 0; frame < filteredImages.size(); frame++) {
			cv::Mat imageCopy = filteredImages[frame].clone();
			if (allIds[frame].size() > 0) {

				if (allCharucoCorners[frame].total() > 0) {
					cv::aruco::drawDetectedCornersCharuco(imageCopy, allCharucoCorners[frame],
						allCharucoIds[frame]);
				}
			}

			imshow("out", imageCopy);
			char key = (char)cv::waitKey(0);
			if (key == 27) break;
		}
	}
	return 0;
}



static bool readDetectorParameters(std::string filename, cv::Ptr<cv::aruco::DetectorParameters> &params) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
	fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
	fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
	fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
	fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
	fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
	fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
	fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
	fs["minDistanceToBorder"] >> params->minDistanceToBorder;
	fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
	fs["doCornerRefinement"] >> params->doCornerRefinement;
	fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
	fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
	fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
	fs["markerBorderBits"] >> params->markerBorderBits;
	fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
	fs["perspectiveRemoveIgnoredMarginPerCell"] >> params->perspectiveRemoveIgnoredMarginPerCell;
	fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
	fs["minOtsuStdDev"] >> params->minOtsuStdDev;
	fs["errorCorrectionRate"] >> params->errorCorrectionRate;
	return true;
}




static bool saveCameraParams(const std::string &filename, cv::Size imageSize, float aspectRatio, int flags,
	const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, const std::vector<cv::Mat> rvecs, const std::vector<cv::Mat> tvecs, double totalAvgErr) {
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

	fs << "camera_cv::Matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	fs << "rvecs" << rvecs;
	fs << "tvecs" << tvecs;
	fs << "avg_reprojection_error" << totalAvgErr;

	return true;
}

