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
		char key = (char)cv::waitKey(10);
		if (key == 27) break;
		if (key == 'c' && ids.size() > 0) {
			std::cout << "Frame captured" << std::endl;
			allCorners.push_back(corners);
			allIds.push_back(ids);
			allImgs.push_back(frame);
			size = frame.size();
		}
	}
	//cv::destroyWindow("out");
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

	/*std::ofstream myfile;
	myfile.open("logPoseEstihgk.txt", std::ios::out | std::ios::app);
	myfile << "\t size of allIdsConcatenated: " << allIdsConcatenated.size() << "\n";
	myfile << "\t size of allCornersConcatenated: " << allCornersConcatenated.size() << "\n";
	myfile << "\t size of markerCounterPerFrame: " << markerCounterPerFrame.size() << "\n";
	myfile << "\t cameraMatrix " << cameraMatrix << "\n";
	myfile << "\t distCoeffs " << distCoeffs << "\n";
	myfile << "\t board " << &board << "\n";
	myfile << "\t size " << size << "\n";
	myfile << "\t calibrationFlags " << calibrationFlags << "\n";


	myfile << "\t Marker \n";
	for (size_t i = 0; i < allIdsConcatenated.size(); i++)
	{
		myfile << "pos " << i << "\n";
		myfile << allIdsConcatenated[i] << "\n";
		std::vector<cv::Point2f> v = allCornersConcatenated[i];
		for (size_t j = 0; j < v.size(); j++)
		{
			myfile << v[j] << "\n";
		}
		myfile << "\n";
	}

	myfile << "\t markerCounterPerFrame \n";
	for (size_t i = 0; i < markerCounterPerFrame.size(); i++)
	{
		myfile << markerCounterPerFrame[i] << "\n";
	}*/
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

	// // FOR DEBUGGING
	//cv::FileStorage fs("CameraMatrix.yml", cv::FileStorage::READ);
	//if (!fs.isOpened())
	//	return false;
	//fs["camera_matrix"] >> cameraMatrix;
	//fs [ "distortion_coefficients" ]>> distCoeffs;

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

//int PoseEstimation::runPoseEstimation(uEye_input* uei)
//{
//	printf("Calibration using a ChArUco board\n");
//	printf("  To capture a frame for calibration, press 'c',\n");
//	printf("  To finish capturing, press 'ESC' key and calibration starts.\n");
//
//	int squaresX =8;
//	int squaresY =8;
//	float squareLength = 0.034;
//	float markerLength = 0.023;
//	int dictionaryId = 16;
//	std::string outputFile = "CameraMatrix.yml";
//	bool fixAspectRatio = false;
//	bool dozeroTangent = false;
//	bool doPrincipalPoint = false;
//	bool showChessboardCorners = false;
//
//	int calibrationFlags = 0;
//	float aspectRatio = 1;
//	if (fixAspectRatio) {
//		calibrationFlags |= cv::CALIB_FIX_ASPECT_RATIO;
//		aspectRatio = 1;
//	}
//	if (dozeroTangent) calibrationFlags |= cv::CALIB_ZERO_TANGENT_DIST;
//	if (doPrincipalPoint) calibrationFlags |= cv::CALIB_FIX_PRINCIPAL_POINT;
//
//	cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();
//	
//	bool refindStrategy = false;
//	
//	cv::Mat frame = uei->getCapturedFrame();
//	cv::Ptr<cv::aruco::Dictionary> dictionary =
//		cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
//
//	// create charuco board object
//	cv::Ptr<cv::aruco::CharucoBoard> charucoboard =
//		cv::aruco::CharucoBoard::create(squaresX, squaresY, squareLength, markerLength, dictionary);
//	cv::Ptr<cv::aruco::Board> board = charucoboard.staticCast<cv::aruco::Board>();
//
//	// collect data from each frame
//	std::vector< std::vector< std::vector< cv::Point2f > > > allCorners;
//	std::vector< std::vector< int > > allIds;
//	std::vector< cv::Mat > allImgs;
//	
//
//	while (!frame.empty()) {
//		cv::Mat imageCopy;
//		frame = uei->getCapturedFrame();
//
//		std::vector< int > ids;
//		std::vector< std::vector< cv::Point2f > > corners, rejected;
//
//		// detect markers
//		cv::aruco::detectMarkers(frame, dictionary, corners, ids, detectorParams, rejected);
//
//		// refind strategy to detect more markers
//		if (refindStrategy) cv::aruco::refineDetectedMarkers(frame, board, corners, ids, rejected);
//
//		// interpolate charuco corners
//		cv::Mat currentCharucoCorners, currentCharucoIds;
//		if (ids.size() > 0)
//			cv::aruco::interpolateCornersCharuco(corners, ids, frame, charucoboard, currentCharucoCorners,
//				currentCharucoIds);
//
//		// draw results
//		frame.copyTo(imageCopy);
//		if (ids.size() > 0) cv::aruco::drawDetectedMarkers(imageCopy, corners);
//
//		if (currentCharucoCorners.total() > 0)
//			cv::aruco::drawDetectedCornersCharuco(imageCopy, currentCharucoCorners, currentCharucoIds);
//
//		putText(imageCopy, "Press 'c' to add current frame. 'ESC' to finish and calibrate",
//			cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
//
//		imshow("out", imageCopy);
//		char key = (char)cv::waitKey(10);
//		if (key == 27) break;
//		if (key == 'c' && ids.size() > 0) {
//			std::cout << "Frame captured" << std::endl;
//			allCorners.push_back(corners);
//			allIds.push_back(ids);
//			allImgs.push_back(frame);
//			size = frame.size();
//		}
//	}
//
//	if (allIds.size() < 1) {
//		std::cerr << "Not enough captures for calibration" << std::endl;
//		return -1;
//	}
//
//	
//	std::vector< cv::Mat > rvecs, tvecs;
//	double repError;
//
//	if (calibrationFlags & cv::CALIB_FIX_ASPECT_RATIO) {
//		cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
//		cameraMatrix.at< double >(0, 0) = aspectRatio;
//	}
//
//	// prepare data for calibration
//	std::vector< std::vector< cv::Point2f > > allCornersConcatenated;
//	std::vector< int > allIdsConcatenated;
//	std::vector< int > markerCounterPerFrame;
//	markerCounterPerFrame.reserve(allCorners.size());
//	for (unsigned int i = 0; i < allCorners.size(); i++) {
//		markerCounterPerFrame.push_back((int)allCorners[i].size());
//		for (unsigned int j = 0; j < allCorners[i].size(); j++) {
//			allCornersConcatenated.push_back(allCorners[i][j]);
//			allIdsConcatenated.push_back(allIds[i][j]);
//		}
//	}
//
//	// calibrate camera using cv::aruco markers
//	double arucoRepErr;
//	arucoRepErr = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
//		markerCounterPerFrame, board, size, cameraMatrix,
//		distCoeffs, cv::noArray(), cv::noArray(), calibrationFlags);
//
//	// prepare data for charuco calibration
//	int nFrames = (int)allCorners.size();
//	std::vector< cv::Mat > allCharucoCorners;
//	std::vector< cv::Mat > allCharucoIds;
//	std::vector< cv::Mat > filteredImages;
//	allCharucoCorners.reserve(nFrames);
//	allCharucoIds.reserve(nFrames);
//
//	for (int i = 0; i < nFrames; i++) {
//		// interpolate using camera parameters
//		cv::Mat currentCharucoCorners, currentCharucoIds;
//		cv::aruco::interpolateCornersCharuco(allCorners[i], allIds[i], allImgs[i], charucoboard,
//			currentCharucoCorners, currentCharucoIds, cameraMatrix,
//			distCoeffs);
//
//		allCharucoCorners.push_back(currentCharucoCorners);
//		allCharucoIds.push_back(currentCharucoIds);
//		filteredImages.push_back(allImgs[i]);
//	}
//
//	if (allCharucoCorners.size() < 4) {
//		std::cerr << "Not enough corners for calibration" << std::endl;
//		return -1;
//	}
//
//	// calibrate camera using charuco
//	repError =
//		cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, size,
//			cameraMatrix, distCoeffs, rvecs, tvecs, calibrationFlags);
//
//
//	bool saveOk = saveCameraParams(outputFile, size, aspectRatio, calibrationFlags,
//		cameraMatrix, distCoeffs,repError);
//	if (!saveOk) {
//		std::cerr << "Cannot save output file" << std::endl;
//		return -1;
//	}
//	std::cout << "Camera Calibration saved to " << outputFile << std::endl;
//
// // // FOR DEBUGGING
//	//cv::FileStorage fs("CameraMatrix.yml", cv::FileStorage::READ);
//	//if (!fs.isOpened())
//	//	return false;
//	//fs["camera_matrix"] >> cameraMatrix;
//	//fs [ "distortion_coefficients" ]>> distCoeffs;
//
//	return 1;
//}

int  PoseEstimation::generateCam2WorldLUT(PlaneCalibration::planeCalibData pcd) {
	// compute CameraWorldMatrix
	computeCamera2World(pcd);

	//compute LUT
	cv::Mat3f lut = computeCamera2WorldLut();

	int saveOk = saveLUT(lut);

	if(saveOk == 1)	return 1;
	
	return -1;

}

void PoseEstimation::computeCamera2World(PlaneCalibration::planeCalibData pcd)
{
	imagePlane = getImagePlane(pcd);
	std::vector<cv::Point3f> worldPlane;
	worldPlane.push_back(cv::Point3f(0.0, 0.0,0.));
	worldPlane.push_back(cv::Point3f(1.0, 0.0,0.));
	worldPlane.push_back(cv::Point3f(0.0, 1.0,0.));
	worldPlane.push_back(cv::Point3f(1.0, 1.0,0.));
	cv::Mat1d rvec = cv::Mat1d(cv::Size(3,1));
	tvec = cv::Mat1d(cv::Size(3, 1));
	rotationMatrix = cv::Mat1d(cv::Size(3, 3));
	/*std::cout << "cameraMatrix \n" << std::endl;
	std::cout << cameraMatrix << std::endl;
	std::cout << "distCoeffs \n" << std::endl;
	std::cout << distCoeffs << std::endl;*/

	cv::solvePnP(worldPlane, imagePlane, cameraMatrix, distCoeffs, rvec, tvec);
	cv::Rodrigues(rvec, rotationMatrix);
	/*std::cout << "rvec \n" << std::endl;
	std::cout << rvec << std::endl;
	std::cout << "tvec \n" << std::endl;
	std::cout << tvec << std::endl;
	std::cout << "rotationMatrix \n" << std::endl;
	std::cout << rotationMatrix << std::endl;*/
}


std::vector<cv::Point2f> PoseEstimation::getImagePlane(PlaneCalibration::planeCalibData pcd)
{
	//oben rechts
	//unten links
	std::vector<cv::Point2f> imagePlane;
	float x1, x2, y1, y2;
	x1 = pcd.upperCorner.x;
	x2 = pcd.lowerCorner.x;
	y1 = pcd.upperCorner.y;
	y2 = pcd.lowerCorner.y;

	imagePlane.push_back(cv::Point(x2, y1)); // l o
	imagePlane.push_back(cv::Point(x1, y1)); // r 0
	imagePlane.push_back(cv::Point(x2,y2)); // l u
	imagePlane.push_back(cv::Point(x1, y2)); // ru
	
	return imagePlane;
}

cv::Mat3f PoseEstimation::computeCamera2WorldLut()
{
	std::cout << "Starting LUT computation. This takes some minutes." << std::endl;
	cv::Mat3f lut = cv::Mat3f(size);
	for (int x1 = 0; x1 < size.width; x1++)
	{ 
		if (x1 == 1024) {
			printf("");
		}
		for (int y = 0; y < size.height; y++)
		{
			cv::Point3f p = PoseEstimation::computeWordCoordinates(cv::Point2f(x1,y), rotationMatrix, cameraMatrix, tvec);
			/*lut.at<cv::Vec3f>(y,x1)[0] = (int)(p.x * 10000 + 0.5) / 10000.0;
			lut.at<cv::Vec3f>(y,x1)[1] = (int)(p.y * 10000 + 0.5) / 10000.0;*/
            lut.at<cv::Vec3f>(y, x1)[0] = p.x;
            lut.at<cv::Vec3f>(y, x1)[1] = p.y;
			lut.at<cv::Vec3f>(y,x1)[2] = 0.;
		}
	}
	std::cout << "Finished LUT computation.\n" << std::endl;
	return lut;
}

cv::Point3f PoseEstimation::computeWordCoordinates(cv::Point2f uv, cv::Mat rotationMatrix, cv::Mat cameraMatrix, cv::Mat tvec) {
	
	cv::Mat uvPoint = cv::Mat::ones(3, 1, cv::DataType<double>::type);
	uvPoint.at<double>(0, 0) = uv.x;
	uvPoint.at<double>(1, 0) = uv.y;
	cv::Mat tempMat, tempMat2;
	double s, zConst = 0;
	// p -> world -> rotated to world
	tempMat = rotationMatrix.inv() * cameraMatrix.inv() * uvPoint;
	// t-vector rotiert
	tempMat2 = rotationMatrix.inv() * tvec;
	s = zConst + tempMat2.at<double>(2, 0);
	s /= tempMat.at<double>(2, 0);
	cv::Mat wcPoint = rotationMatrix.inv() * (s * cameraMatrix.inv() * uvPoint - tvec);
	return cv::Point3f(wcPoint.at<double>(0, 0), wcPoint.at<double>(1, 0), wcPoint.at<double>(2, 0));

}

bool PoseEstimation::saveLUT(const cv::Mat3f lut) {
	cv::FileStorage fs("LUT.yml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;
	time_t tt;
	time(&tt);
	struct tm *t2 = localtime(&tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);
	fs << "calibration_time" << buf;
	fs << "LUT" << lut;
	return true;
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
	//fs << "distortion_coefficients" << distCoeffs;
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