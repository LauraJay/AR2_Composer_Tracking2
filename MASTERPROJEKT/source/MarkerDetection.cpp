#include "MarkerDetection.h"

MarkerDetection::MarkerDetection()
{
	initArucoParams();
}

int MarkerDetection::runMarkerDetection(cv::Mat &frame)
{
	detectedRects.clear();
	arucoIds.clear();
	cv::Mat colorThresImg = colorThreshold(frame);
	detectedRects = detectMarkerRectangles(colorThresImg);
	detectArucoMarker(frame);
	return 1;
}

std::vector<cv::RotatedRect> MarkerDetection::getDetectedRects()
{
	return detectedRects;
}

std::vector<int> MarkerDetection::getArucoIds()
{
	return arucoIds;
}

std::vector<std::vector<cv::Point2f>> MarkerDetection::getArucoCorners()
{
	return corners;
}

cv::Mat MarkerDetection::colorThreshold(cv::Mat &frame) {

	cv::Mat output;
	cvtColor(frame, output, cv::COLOR_BGR2HSV);
	inRange(output, cv::Scalar(35, 60, 80), cv::Scalar(65, 255, 255), output);
	int erosion_size = 1;
	cv::Mat element = getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));

	morphologyEx(output, output, cv::MORPH_OPEN, element, cv::Point(-1, -1));
	morphologyEx(output, output, cv::MORPH_CLOSE, element, cv::Point(-1, -1));
	return output;
}


std::vector<cv::RotatedRect> MarkerDetection::detectMarkerRectangles(cv::Mat &colorThresImg)
{
	std::vector<cv::Point> points;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Canny(colorThresImg, colorThresImg, 300, 600, 3);
	findContours(colorThresImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	std::vector<cv::RotatedRect> box;
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		cv::RotatedRect r = minAreaRect(cv::Mat(contours_poly[i]));
		if (r.size.height>10 && r.size.width>10) box.push_back(r);
	}

	return box;
}

void MarkerDetection::initArucoParams()
{
	dictionaryId = cv::aruco::DICT_4X4_50;
	showRejected = false; // zeigt die fehlerhaften Marker
	estimatePose = false; // use CameraCalib
	markerLength = 0.02; // size of outprinted Marker
	detectorParams = cv::aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers
	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	if (estimatePose) {
		cv::FileStorage fs("filename", cv::FileStorage::READ);
		fs["camera_matrix"] >> camMatrix;
		fs["distortion_coefficients"] >> distCoeffs;
	}
}

void MarkerDetection::detectArucoMarker(cv::Mat &frame)
{
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	// detect markers and estimate pose
	if (estimatePose && arucoIds.size() > 0)
		cv::aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs,tvecs);
	// draw results TODEBUG
	/*cv::Mat imageCopy;
	frame.copyTo(imageCopy);
	if (arucoIds.size() > 0) {
		cv::aruco::drawDetectedMarkers(imageCopy, corners, arucoIds);

		if (estimatePose) {
			for (unsigned int i = 0; i < arucoIds.size(); i++)
				cv::aruco::drawAxis(imageCopy, camMatrix, distCoeffs, rvecs[i], tvecs[i],
					markerLength * 0.5f);
		}
	}

	if (showRejected && rejected.size() > 0)
		cv::aruco::drawDetectedMarkers(imageCopy, rejected, cv::noArray(), cv::Scalar(100, 0, 255));

	imshow("out", imageCopy);
	cv::waitKey(1);*/

}


