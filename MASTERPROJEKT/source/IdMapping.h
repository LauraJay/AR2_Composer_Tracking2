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

	int isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	void CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);
	int isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	int isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isMarkerOutOfField();
};
