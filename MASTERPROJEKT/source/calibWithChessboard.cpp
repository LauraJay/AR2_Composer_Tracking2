#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <calibWithChessboard.h>
#include <cctype>
#include <stdio.h>
#include <string.h>
#include <time.h>


using namespace cv;
using namespace std;




enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

static double computeReprojectionErrors(
	const vector<vector<Point3f> >& objectPoints,
	const vector<vector<Point2f> >& imagePoints,
	const vector<Mat>& rvecs, const vector<Mat>& tvecs,
	const Mat& cameraMatrix, const Mat& distCoeffs,
	vector<float>& perViewErrors)
{
	vector<Point2f> imagePoints2;
	int i, totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for (i = 0; i < (int)objectPoints.size(); i++)
	{
		projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
			cameraMatrix, distCoeffs, imagePoints2);
		err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
		int n = (int)objectPoints[i].size();
		perViewErrors[i] = (float)std::sqrt(err*err / n);
		totalErr += err*err;
		totalPoints += n;
	}

	return std::sqrt(totalErr / totalPoints);
}

static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD)
{
	corners.resize(0);

	switch (patternType)
	{
	case CHESSBOARD:
	case CIRCLES_GRID:
		for (int i = 0; i < boardSize.height; i++)
			for (int j = 0; j < boardSize.width; j++)
				corners.push_back(Point3f(float(j*squareSize),
					float(i*squareSize), 0));
		break;

	case ASYMMETRIC_CIRCLES_GRID:
		for (int i = 0; i < boardSize.height; i++)
			for (int j = 0; j < boardSize.width; j++)
				corners.push_back(Point3f(float((2 * j + i % 2)*squareSize),
					float(i*squareSize), 0));
		break;

	default:
		CV_Error(Error::StsBadArg, "Unknown pattern type\n");
	}
}

static bool runCalibration(vector<vector<Point2f> > imagePoints,
	Size imageSize, Size boardSize, Pattern patternType,
	float squareSize, float aspectRatio,
	int flags, Mat& cameraMatrix, Mat& distCoeffs,
	vector<Mat>& rvecs, vector<Mat>& tvecs,
	vector<float>& reprojErrs,
	double& totalAvgErr)
{
	cameraMatrix = Mat::eye(3, 3, CV_64F);
	if (flags & CALIB_FIX_ASPECT_RATIO)
		cameraMatrix.at<double>(0, 0) = aspectRatio;

	distCoeffs = Mat::zeros(8, 1, CV_64F);

	vector<vector<Point3f> > objectPoints(1);
	calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

	objectPoints.resize(imagePoints.size(), objectPoints[0]);

	double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
		distCoeffs, rvecs, tvecs, flags | CALIB_FIX_K4 | CALIB_FIX_K5);


	///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
	printf("RMS error reported by calibrateCamera: %g\n", rms);

	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

	totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
		rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

	return ok;
}




static bool readStringList(const string& filename, vector<string>& l)
{
	l.resize(0);
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
		return false;
	FileNode n = fs.getFirstTopLevelNode();
	if (n.type() != FileNode::SEQ)
		return false;
	FileNodeIterator it = n.begin(), it_end = n.end();
	for (; it != it_end; ++it)
		l.push_back((string)*it);
	return true;
}


static bool runAndSave(const string& outputFilename,
	const vector<vector<Point2f> >& imagePoints,
	Size imageSize, Size boardSize, Pattern patternType, float squareSize,
	float aspectRatio, int flags, Mat& cameraMatrix,
	Mat& distCoeffs, vector<Mat>& rvecs, vector<Mat>&tvecs,
	bool writeExtrinsics, bool writePoints)
{
	
	vector<float> reprojErrs;
	double totalAvgErr = 0;

	bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
		aspectRatio, flags, cameraMatrix, distCoeffs,
		rvecs, tvecs, reprojErrs, totalAvgErr);
	printf("%s. avg reprojection error = %.2f\n",
		ok ? "Calibration succeeded" : "Calibration failed",
		totalAvgErr);

	return ok;
}


int calibWithChessboard::runCalibWithChessboard(uEye_input* uei)
{

	printf("Cheesboard Calibration starts ...");
	printf("When the live video from camera is used as input, the following hot-keys may be used:\n"
	"  <ESC>, 'q' - quit the program\n"
	"  'g' - start capturing images\n"
	"  'u' - switch undistortion on/off\n"
	"This calibration requiers 25 captures. While capturing each sucessful capture is verified by a short beep.\n"
	"When the complete capturing of all imagees is verfied by a long beep ");
	Size boardSize, imageSize;
	float squareSize, aspectRatio;
	//Mat cameraMatrix, distCoeffs;
	string outputFilename;
	string inputFilename = "";

	int i, nframes;
	bool writeExtrinsics, writePoints;
	bool undistortImage = false;
	int flags = 0;
	//VideoCapture capture;

	bool flipVertical;
	bool showUndistorted;
	bool videofile;
	int delay;
	clock_t prevTimestamp = 0;
	int mode = DETECTION;
	int cameraId = 0;
	vector<vector<Point2f> > imagePoints;
	vector<string> imageList;
	Pattern pattern = CHESSBOARD;

	
	boardSize.width = 7;
	boardSize.height = 5;
	pattern = CHESSBOARD;
	
	squareSize = 0.027;
	nframes = 25;
	aspectRatio = 1;
	delay = 1000;
	writePoints = true;
	writeExtrinsics = true; 
	bool isready = false;
	
	flipVertical = false;
	videofile = "";
	outputFilename = "instrinctCameraParams";
	showUndistorted = true;
	cameraId = 0;
	inputFilename = "";
	
	if (squareSize <= 0)
		return fprintf(stderr, "Invalid board square width\n"), -1;
	if (nframes <= 3)
		return printf("Invalid number of images\n"), -1;
	if (aspectRatio <= 0)
		return printf("Invalid aspect ratio\n"), -1;
	if (delay <= 0)
		return printf("Invalid delay\n"), -1;
	if (boardSize.width <= 0)
		return fprintf(stderr, "Invalid board width\n"), -1;
	if (boardSize.height <= 0)
		return fprintf(stderr, "Invalid board height\n"), -1;


	namedWindow("Image View", 1);

	for (i = 0;; i++)
	{
		Mat view, viewGray;
		bool blink = false;

		if (uei->isActive())
		{
			Mat view0;
			view0 = uei->getCapturedFrame();
			view0.copyTo(view);
		}
		else if (i < (int)imageList.size())
			view = imread(imageList[i], 1);

		if (view.empty())
		{
			if (imagePoints.size() > 0)
				runAndSave(outputFilename, imagePoints, imageSize,
					boardSize, pattern, squareSize, aspectRatio,
					flags, cameraMatrix, distCoeffs, rvecs,tvecs,
					writeExtrinsics, writePoints);
			break;
		}

		imageSize = view.size();

		if (flipVertical)
			flip(view, view, 0);

		vector<Point2f> pointbuf;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);

		bool found;
		switch (pattern)
		{
		case CHESSBOARD:
			found = findChessboardCorners(view, boardSize, pointbuf,
				CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
			break;
		case CIRCLES_GRID:
			found = findCirclesGrid(view, boardSize, pointbuf);
			break;
		case ASYMMETRIC_CIRCLES_GRID:
			found = findCirclesGrid(view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID);
			break;
		default:
			return fprintf(stderr, "Unknown pattern type\n"), -1;
		}

		// improve the found corners' coordinate accuracy
		if (pattern == CHESSBOARD && found) cornerSubPix(viewGray, pointbuf, Size(11, 11),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));

		if (mode == CAPTURING && found &&
			(!uei->isActive() || clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC))
		{
			imagePoints.push_back(pointbuf);
			printf("Size of Capture: %d \n",imagePoints.size());
			prevTimestamp = clock();
			blink = uei->isActive();
			Beep(500, 500);
		}

		if (found)
			drawChessboardCorners(view, boardSize, Mat(pointbuf), found);

		string msg = mode == CAPTURING ? "100/100" :
			mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
		int baseLine = 0;
		Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
		Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

		if (mode == CAPTURING)
		{
			if (undistortImage)
				msg = format("%d/%d Undist", (int)imagePoints.size(), nframes);
			else
				msg = format("%d/%d", (int)imagePoints.size(), nframes);
		}

		putText(view, msg, textOrigin, 1, 1,
			mode != CALIBRATED ? Scalar(0, 0, 255) : Scalar(0, 255, 0));

		if (blink) {
			bitwise_not(view, view);
			}

		if (mode == CALIBRATED && undistortImage)
		{
			Mat temp = view.clone();
			undistort(temp, view, cameraMatrix, distCoeffs);
		}

		imshow("Image View", view);

		char key = (char)waitKey(uei->isActive() ? 50 : 500);

		if (key == 27)
			break;

		if (key == 'u' && mode == CALIBRATED)
			undistortImage = !undistortImage;

		if (uei->isActive() && key == 'g')
		{
			mode = CAPTURING;
			printf("isCapuring \n");
			imagePoints.clear();
		}
		
		if (mode == CAPTURING && imagePoints.size() >= (unsigned)nframes)
		{
			Beep(800, 800);

			if (runAndSave(outputFilename, imagePoints, imageSize,
				boardSize, pattern, squareSize, aspectRatio,
				flags, cameraMatrix, distCoeffs, rvecs, tvecs,
				writeExtrinsics, writePoints)) {
			mode = CALIBRATED;
			isready = true;
			
		}
			else
				mode = DETECTION;
			if (!uei->isActive())
				break;
		}


	if (isready)break;
	}

	if (!uei->isActive() && showUndistorted)
	{
		Mat view, rview, map1, map2;
		initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
			getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
			imageSize, CV_16SC2, map1, map2);

		for (i = 0; i < (int)imageList.size(); i++)
		{
			view = imread(imageList[i], 1);
			if (view.empty())
				continue;
			//undistort( view, rview, cameraMatrix, distCoeffs, cameraMatrix );
			remap(view, rview, map1, map2, INTER_LINEAR);
			imshow("Image View", rview);
			char c = (char)waitKey();
			if (c == 27 || c == 'q' || c == 'Q')
				break;
		}
	}
	printf("... Cheesboard Calibration finished");
	return 0;
}
