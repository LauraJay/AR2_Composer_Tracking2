#include "MarkerDetection.h"

MarkerDetection::MarkerDetection()
{
	initArucoParams();
}

// calls the green rectangle detection and the aruco marker detection
// @param cv::Mat frame: the current frame 
// @return int: 1 if sucessful
int MarkerDetection::runMarkerDetection(cv::Mat frame)
{
	detectedRects.clear();
	arucoIds.clear();
	// green threshold
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

// green keying of current frame
// @param cv::Mat frame: the current frame 
// @return cv::Mat: green keyed image
cv::Mat MarkerDetection::colorThreshold(cv::Mat &frame) {

	cv::Mat output;
	cvtColor(frame, output, cv::COLOR_RGB2HSV);
	inRange(output, cv::Scalar(60, 120, 22), cv::Scalar(85, 255, 255), output);
	/*cv::namedWindow("inRange", cv::WINDOW_KEEPRATIO);	
	cv::imshow("inRange", output);
	cv::waitKey(1);*/
	cv::medianBlur(output, output, 5);
	/*cv::namedWindow("Threshold", cv::WINDOW_KEEPRATIO);    
    cv::imshow("Threshold", output);
	cv::waitKey(1);*/
	/*int erosion_size = 3;
	cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));

	morphologyEx(output, output, cv::MORPH_OPEN, element, cv::Point(-1, -1));*/
	return output;
}


// generates MarkerRectangles in form of object alligned bounding boxes
// @param cv::Mat colorThresImg: green keyed image
// @return std::vector<cv::RotatedRect>: vector of object alligned bounding boxes
std::vector<cv::RotatedRect> MarkerDetection::detectMarkerRectangles(cv::Mat &colorThresImg)
{
	std::vector<cv::Point> points;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Canny(colorThresImg, colorThresImg, 600, 1200, 3);
	/*cv::namedWindow("Canny", cv::WINDOW_KEEPRATIO);
	cv::imshow("Canny", colorThresImg);
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
		if ((r.size.height < 70 && r.size.width < 70) && (r.size.height > 30 && r.size.width > 30)) box.push_back(r);
	}
	return box;
}

// initalisation of aruco marker detection
void MarkerDetection::initArucoParams()
{
	dictionaryId = cv::aruco::DICT_4X4_50; //choose the dictonary you want to use.
	showRejected = false; // displays the rejected markers
	markerLength = 0.025; // size of outprinted Aruco Markers in meters
	detectorParams = cv::aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers
	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	}


// aruco marker detection
// @param cv::Mat frame: the current frame 
void MarkerDetection::detectArucoMarker(cv::Mat & frame) {
	cv::aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams);
	std::vector< cv::Vec3d > rvecs, tvecs;	// detect markers and estimate pose
	}


