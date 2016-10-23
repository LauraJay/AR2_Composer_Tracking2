#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>
#include <MarkerManagement.h>

using namespace cv;

class IdMapping {

private:


public:
	int finalID;
	IdMapping();
	~IdMapping();
	void CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);

	bool isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	void CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);
	bool isConstantMarker(std::vector<cv::Point2f> points, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	std::vector<Marker*> IdMapping::CurrentMarker(std::vector<Marker*> tm, std::vector<Point2f> rectPoints, Point2f center, unsigned char markedCorner);
	bool isMarkerOutOfField();
	bool isTrackedMarker(std::vector<Point2f> points, unsigned char markedCorner);
};
