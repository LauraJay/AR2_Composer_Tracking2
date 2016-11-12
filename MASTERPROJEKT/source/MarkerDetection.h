#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace cv;
class MarkerDetection
{
private:
	Mat frame;
	Mat colorThreshold(Mat frame);
	std::vector<RotatedRect> detectedRects;
	std::vector<int> arucoIds;
	vector< Vec3d > rvecs, tvecs;
	std::vector<RotatedRect> getOBB(Mat colorThresImg);
	std::vector<RotatedRect> detectMarkerRectangles(Mat frame);
	int isRectWithArucoMarker(RotatedRect rect, Size imgSize);
	int dictionaryId;
	bool showRejected;
	bool estimatePose;
	float markerLength;
	Ptr<aruco::DetectorParameters> detectorParams;
	Ptr<aruco::Dictionary> dictionary;
	Mat camMatrix;
	Mat distCoeffs;
	void initArucoParams();
	void detectArucoMarker(Mat frame);

protected:

	vector<vector<cv::Point2f>> corners;
	vector<vector<cv::Point2f>> rejected;
public:
	MarkerDetection();
	~MarkerDetection();
	int runMarkerDetection(Mat &frame);
	std::vector<RotatedRect> getDetectedRects();
	std::vector<int> getArucoIds();
};

