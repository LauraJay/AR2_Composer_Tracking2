#include "MarkerDetection.h"
using namespace cv;
using namespace std;



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
	Mat colorThresImg = colorThreshold(frame);
	std::vector<RotatedRect>  box = detectMarkerRectangles(colorThresImg);
	detectArucoMarker(frame);
	
	if (!box.empty()) {
		for (int i = 0; i < box.size(); i++)
		{
			RotatedRect rect = box[i];
			int ret = isRectWithArucoMarker(rect,Size(frame.size()));
			//testen ob Aruco in Rect
			//testen ob Rect richtige Größe
			//testen ob übrige Rect zusammengehören (Teilung durch Finger)


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

Mat MarkerDetection::colorThreshold(Mat frame) {

	Mat output;
	cvtColor(frame, output, COLOR_BGR2HSV);
	inRange(output, Scalar(55, 0, 50), Scalar(110, 255, 255), output);
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

std::vector<RotatedRect> MarkerDetection::detectMarkerRectangles(Mat colorThresImg)
{
	std::vector<RotatedRect>  box = getOBB(colorThresImg.clone());
	return box;
}

int MarkerDetection::isRectWithArucoMarker(RotatedRect rect, Size imgSize)
{
	Mat image;
	image.create(imgSize, CV_8UC3);
	image.setTo(0);
	cv::Point2f vertices2f[4];
	cv::Point vertices[4];
	rect.points(vertices2f);
	for (int i = 0; i < 4; ++i) {
		vertices[i] = vertices2f[i];
	}
	cv::fillConvexPoly(image,vertices,4,Scalar(255,255,255),-1);
	for (int i = 0; i < arucoIds.size(); i++)
	{
		for (int j = 0; j < corners[j].size(); j++)
		{
			// test if all Points are White
		}

	}


	return 1;
}

void MarkerDetection::initArucoParams()
{
	dictionaryId = aruco::DICT_4X4_50;
	showRejected = true; // zeigt die fehlerhaften Marker
	estimatePose = false; // use CameraCalib
	markerLength = 0.025; // size of outprinted Marker
	detectorParams = aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers
	dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	if (estimatePose) {
		FileStorage fs("filename", FileStorage::READ);
		fs["camera_matrix"] >> camMatrix;
		fs["distortion_coefficients"] >> distCoeffs;
	}
}

void MarkerDetection::detectArucoMarker(Mat frame)
{
	Mat img;
	frame.copyTo(img);
	Mat blurImg;
	GaussianBlur(img, blurImg, cv::Size(0, 0), 3);
	addWeighted(img, 1.5, blurImg, -0.5, 0, blurImg);	
	aruco::detectMarkers(frame, dictionary, corners, arucoIds, detectorParams, rejected);
	// detect markers and estimate pose
	if (estimatePose && arucoIds.size() > 0)
		aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs,tvecs);
	// draw results TODEBUG
	Mat imageCopy;
	frame.copyTo(imageCopy);
	if (arucoIds.size() > 0) {
		aruco::drawDetectedMarkers(imageCopy, corners, arucoIds);

		if (estimatePose) {
			for (unsigned int i = 0; i < arucoIds.size(); i++)
				aruco::drawAxis(imageCopy, camMatrix, distCoeffs, rvecs[i], tvecs[i],
					markerLength * 0.5f);
		}
	}

	if (showRejected && rejected.size() > 0)
		aruco::drawDetectedMarkers(imageCopy, rejected, noArray(), Scalar(100, 0, 255));

	imshow("out", imageCopy);
	waitKey(1);

}


