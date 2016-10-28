#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <Marker.h>
#include <MarkerManagement.h>

using namespace cv;
class IdMapping {

private:
	std::ofstream myfile2;
	
public:
	std::vector<Point2f> IdMapping::CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> trackedMarker);
	std::vector<Point2f>  CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> trackedMarker);
	int isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, std::vector<Point2f> motionCenterVecs, std::vector<Point2f> motionMarkCornerVecs);
	//static int isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, std::vector<Point2f> motionCenterVecs, std::vector<Point2f> motionMarkCornerVecs);
	int isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, std::vector<Point2f> motionCenterVecs, std::vector<Point2f> motionMarkCornerVecs);
	bool isMarkerOutOfField();
	IdMapping();
	~IdMapping();
};
