#include "MarkerDetection.h"

MarkerDetection::MarkerDetection()
{
	initArucoParams();
}

int MarkerDetection::runMarkerDetection(cv::Mat frame)
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
	cvtColor(frame, output, cv::COLOR_RGB2HSV);
	inRange(output, cv::Scalar(60, 120, 15), cv::Scalar(85, 255, 255), output);
	cv::medianBlur(output, output, 3);
  /*  cv::imshow("Threshold", output);
    cv::waitKey(1);*/
	/*int erosion_size = 3;
	cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));

	morphologyEx(output, output, cv::MORPH_OPEN, element, cv::Point(-1, -1));*/
	return output;
}


std::vector<cv::RotatedRect> MarkerDetection::detectMarkerRectangles(cv::Mat &colorThresImg)
{
	std::vector<cv::Point> points;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Canny(colorThresImg, colorThresImg, 600, 1200, 5);
	/*cv::imshow("Canny", colorThresImg);
	cv::waitKey(1);*/
	findContours(colorThresImg, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	std::vector<cv::RotatedRect> box;
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 0.1, true);
		cv::RotatedRect r = minAreaRect(cv::Mat(contours_poly[i]));
		r.center = cv::Point2i(r.center);
		r.size = cv::Size2i(r.size);
		if (r.size.height>40||r.size.width>40) box.push_back(r);
		/*if (r.size.width > 55 || r.size.width < 45 || r.size.height > 55 || r.size.height < 45) {
			if (r.size.height>30 && r.size.width>30)
			printf("");
		}*/
	}

	return box;
}

void MarkerDetection::initArucoParams()
{
	dictionaryId = cv::aruco::DICT_4X4_50;
	showRejected = false; // zeigt die fehlerhaften Marker
	markerLength = 0.025; // size of outprinted Marker
	detectorParams = cv::aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = false; // do corner refinement in markers
	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	}



void MarkerDetection::detectArucoMarker(cv::Mat & frame) {
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams);
	//std::vector< cv::Vec3d > rvecs, tvecs;	// detect markers and estimate pose
	/*if (arucoIds.size() > 0)
	cv::aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs,tvecs);*/

	//// draw results TODEBUG
	//cv::Mat imageCopy;
	//frame.copyTo(imageCopy);
	//if (arucoIds.size() > 0) {
	//	cv::aruco::drawDetectedMarkers(imageCopy, corners, arucoIds);
	//		for (unsigned int i = 0; i < arucoIds.size(); i++)
	//			cv::circle(imageCopy, corners[i][0], 3, cv::Scalar(255, 0, 255));
	//	
	//}

	/*imshow("corners", imageCopy);
	cv::waitKey(1);*/

}


