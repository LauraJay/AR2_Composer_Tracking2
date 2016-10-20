#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>

class IdMapping {

private:


public:
	IdMapping();
	~IdMapping();
	std::vector<Point2f> CalculateMotionVector(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm);
	bool MotionEstimationPerMarker(std::vector<Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isConstantMarker(std::vector<cv::Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isMarkerOutOfField();
	bool isTrackedMarker(std::vector<Point2f> points, unsigned char markedCorner);
	

};
