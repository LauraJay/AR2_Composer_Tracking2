#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>

class IdMapping {

private:
	

public:
	IdMapping();
	~IdMapping();
	bool isConstantMarker(std::vector<cv::Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm);
};