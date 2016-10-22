#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>

using namespace cv;

class IdMapping {

private:


public:
	IdMapping();
	~IdMapping();
	void CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);
	void CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);
	void MotionEstimationPerMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
//	bool isConstantMarker(std::vector<cv::Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isConstantMarker(void);
	bool isMarkerOutOfField();
	bool isTrackedMarker(std::vector<Point2f> points, unsigned char markedCorner);
	

};
