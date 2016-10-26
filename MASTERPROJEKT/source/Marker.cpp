#include "Marker.h"


float Marker::computeAngle(unsigned char markCornerID, std::vector<cv::Point2f> rectPoints)
{
	float angleRad;
	float angleGrad;
	const double PI(3.14159265);
	cv::Point2f unitVector(100, 0);
	
	cv::Point2f orientationVector = rectPoints[markCornerID] - center;
	//Calculate the angle between the unit vector and the vector between the center and the green corner
	float scalar = (unitVector.x * orientationVector.x) + (unitVector.y * orientationVector.y);
	float distance1 = sqrt((unitVector.x * unitVector.x) + (unitVector.y * unitVector.y));
	float distance2 = sqrt((orientationVector.x * orientationVector.x) + (orientationVector.y * orientationVector.y));
	angleRad = acos(scalar / (distance1 * distance2));
	
	//Case 1: Green Corner lies above the center of the box
	if (rectPoints[markCornerID].y <= center.y) {
		angleGrad = angleRad * 180 / PI;
	}
	//Case 2: Green Corner lies below the center of the box
	if (rectPoints[markCornerID].y > center.y) {
		angleGrad = 360 - (angleRad * 180 / PI);
	}
	return angleGrad;
}

Marker::Marker(int id, std::vector<cv::Point2f> rect, cv::Point2f center, unsigned char markCornerID)
{
	Marker::id = id;
	rectPoints = rect;
	Marker::markCornerID = markCornerID;
	Marker::center = center;
	angle = computeAngle(markCornerID, rectPoints);
}

Marker::~Marker()
{
}

cv::Point2f Marker::getMotionCenterVec()
{
	return motionCenterVec;
}

cv::Point2f Marker::getMotionMarkCornerVec()
{
	return motionMarkCornerVec;
}

int Marker::getId()
{
	return id;
}

int* Marker::getIdPointer()
{
	return &id;
}

std::vector<cv::Point2f> Marker::getPoints()
{
	return rectPoints;
}

unsigned char Marker::getMarkedCornerID()
{
	return markCornerID;
}

cv::Point2f Marker::getCenter()
{
	return center;
}

float Marker::getAngle()
{
	return angle;
}

void Marker::setMotionCenterVec(cv::Point2f motionCenterVec)
{
	Marker::motionCenterVec = motionCenterVec;
}

void Marker::setMotionMarkCornerVec(cv::Point2f motionMarkCornerVec)
{
	Marker::motionMarkCornerVec = motionMarkCornerVec;
}

void Marker::setPoints(std::vector<cv::Point2f>  rect, cv::Point2f c)
{	
	rectPoints = rect;
	center = c;
}

void Marker::setMarkedCornerID(int id)
{
	Marker::markCornerID = id;
}

void Marker::setAngle(float angle)
{
	Marker::angle = angle;
}

void Marker::setId(int id)
{
	Marker::id = id;
}


