#include "MarkerDetection.h"
using namespace cv;



MarkerDetection::MarkerDetection()
{
	initArucoParams();
}


MarkerDetection::~MarkerDetection()
{
}

int MarkerDetection::runMarkerDetection(Mat &frame)
{
	detectedRects.clear();
	arucoIds.clear();

	std::vector<RotatedRect>  box = detectMarkerRectangles(frame);
	detectArucoMarker(frame);
	
	if (!box.empty()) {



		
		for (int i = 0; i < box.size(); i++)
		{
			
			/*if (corner < 5) {
				markedCorners.push_back(corner);
				detectedRects.push_back(box[i]);
			}*/
			//else return 0;
		}
	}
	return 1;
}

std::vector<RotatedRect> MarkerDetection::getDetectedRects()
{


	return detectedRects;
}

std::vector<int> MarkerDetection::getArucoIds()
{
	return arucoIds;
}

Mat MarkerDetection::colorThreshold(Mat &frame) {
	Mat output;
	inRange(frame, Scalar(55, 0, 50), Scalar(110, 255, 255), output);
	int erosion_size = 1;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	morphologyEx(output, output, MORPH_OPEN, element, Point(-1, -1));
	morphologyEx(output, output, MORPH_CLOSE, element, Point(-1, -1));
	return output;
}

std::vector<RotatedRect>  MarkerDetection::getOBB(Mat colorThresImg) {
	std::vector<Point> points;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Canny(colorThresImg, colorThresImg, 100, 600, 3);
	findContours(colorThresImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<RotatedRect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = minAreaRect(Mat(contours_poly[i]));
	}
	return boundRect;
}

std::vector<RotatedRect> MarkerDetection::detectMarkerRectangles(Mat frame)
{
	Mat colorThresImg = colorThreshold(frame);
	std::vector<RotatedRect>  box = getOBB(colorThresImg.clone());
	return box;
}

void MarkerDetection::initArucoParams()
{
	dictionaryId = aruco::DICT_4X4_50;
	showRejected = false;// zeige verworfene Marker an
	estimatePose = false; //
	float markerLength = 0.025; //ausgedruckte Größe
	detectorParams = aruco::DetectorParameters::create();
	detectorParams->adaptiveThreshWinSizeMin = 3;
	detectorParams->adaptiveThreshWinSizeMax = 23;
	detectorParams->adaptiveThreshWinSizeStep = 10;
	detectorParams->adaptiveThreshConstant = 7;
	detectorParams->minMarkerPerimeterRate = 0.03;
	detectorParams->maxMarkerPerimeterRate = 4.0;
	detectorParams->polygonalApproxAccuracyRate = 0.05;
	detectorParams->minCornerDistanceRate = 10.0;
	detectorParams->minDistanceToBorder = 3;
	detectorParams->minMarkerDistanceRate = 0.05;
	detectorParams->doCornerRefinement = false;
	detectorParams->cornerRefinementWinSize = 5;
	detectorParams->cornerRefinementMaxIterations = 30;
	detectorParams->cornerRefinementMinAccuracy = 0.1;
	detectorParams->markerBorderBits = 1;
	detectorParams->perspectiveRemovePixelPerCell = 8;
	detectorParams->perspectiveRemoveIgnoredMarginPerCell = 0.13;
	detectorParams->maxErroneousBitsInBorderRate = 0.04;
	detectorParams->minOtsuStdDev = 5.0;
	detectorParams->errorCorrectionRate = 0.6;
	dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	// nur mit Kamerakalibierung benutzten!!!
	if (estimatePose) {
		//bool readOk = readCameraParameters(parser.get<string>("c"), camMatrix, distCoeffs);
		/*if (!readOk) {
		cerr << "Invalid camera file" << endl;
		return 0;
		}*/
	}
}

void MarkerDetection::detectArucoMarker(Mat frame)
{
	std::vector< int > ids = {};
	std::vector< std::vector< Point2f > > corners
		= {};
	std::vector< std::vector< Point2f > > rejected = {};
	std::vector< Vec3d > rvecs, tvecs;
	Mat rgb;
	cvtColor(frame, rgb, COLOR_HSV2BGR);
	// detect markers and estimate pose
	aruco::detectMarkers(rgb, dictionary, corners, ids, detectorParams, rejected);

	if (estimatePose && ids.size() > 0)
		aruco::estimatePoseSingleMarkers(corners, arucoMarkerLength, camMatrix, distCoeffs, rvecs,tvecs);
	
	// draw results
	Mat imageCopy;
	frame.copyTo(imageCopy);
	if (ids.size() > 0) {
		aruco::drawDetectedMarkers(imageCopy, corners, ids);

		if (estimatePose) {
			for (unsigned int i = 0; i < ids.size(); i++)
				aruco::drawAxis(imageCopy, camMatrix, distCoeffs, rvecs[i], tvecs[i],
					arucoMarkerLength * 0.5f);
		}
	}

	if (showRejected && rejected.size() > 0)
		aruco::drawDetectedMarkers(imageCopy, rejected, noArray(), Scalar(100, 0, 255));

	imshow("out", imageCopy);
	waitKey(4);



}


