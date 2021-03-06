#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include<PoseEstimation.h>
class MarkerDetection
{

private:
	cv::Mat frame;
	cv::Mat colorThreshold(cv::Mat &frame);
	std::vector<cv::RotatedRect> detectedRects;
	std::vector<int> arucoIds;
	int dictionaryId;
	bool showRejected;
	float markerLength;
	cv::Mat camMatrix;
	cv::Mat distCoeffs;
	std::vector<cv::RotatedRect> detectMarkerRectangles(cv::Mat &frame);
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	void initArucoParams();
	void detectArucoMarker(cv::Mat &frame);
protected:

	std::vector<std::vector<cv::Point2f>> corners;
public:
	MarkerDetection();
	~MarkerDetection() {};
	int runMarkerDetection(cv::Mat frame);
	std::vector<cv::RotatedRect> getDetectedRects();
	std::vector<int> getArucoIds();
	std::vector<std::vector<cv::Point2f>> getArucoCorners();

};

