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
	Point2f mvC;
	Point2f mvMC;
	float x;
	float y;
	//Müssen wir noch automatisieren über die Größe der Marker (wie besprochen)
	float tCenterConstant = 0.03f;
	float tMarkedCornerConstant = 0.05f;
	float tTranslation = 0.2f;
	float tRotattion = 0.03f;

	void CalculateMotionVectorCenter(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);
	void CalculateMotionVectorMarkedCorner(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm, int nr);

public:
	int isConstantMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	int isRotatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	int isTranslatedMarker(std::vector<Point2f> points, Point2f center, unsigned char markedCorner, std::vector<Marker*> tm);
	bool isMarkerOutOfField();
	IdMapping();
	~IdMapping();
};
