#include "Marker.h"


float Marker::computeAngle(cv::Point2f angelPoint, cv::Point2f center)
{
	float angleRad;
	float angleGrad;
	const double PI(3.14159265);
	cv::Point2f temp = cv::Point(center.x-20, center.y);
	cv::Point2f unitVector = temp - center;
	cv::Point2f orientationVector = angelPoint - center;
	//Calculate the angle between the unit vector and the vector between the center and the green corner
	float scalar = (unitVector.x * orientationVector.x) + (unitVector.y * orientationVector.y);
	float distance1 = sqrt((unitVector.x * unitVector.x) + (unitVector.y * unitVector.y));
	float distance2 = sqrt((orientationVector.x * orientationVector.x) + (orientationVector.y * orientationVector.y));
	angleRad = acos(scalar / (distance1 * distance2));
	
	//Case 1: Green Corner lies above the center of the box
	if (angelPoint.y <= rect.center.y) {
		angleGrad = angleRad * 180 / PI;
	}
	//Case 2: Green Corner lies below the center of the box
	if (angelPoint.y > rect.center.y) {
		angleGrad = 360 - (angleRad * 180 / PI);
	}

	angleGrad -= 45;
	if (angleGrad < 0) angleGrad  = 360 + angleGrad;
	return  (int) angleGrad;
}

Marker::Marker(int id, cv::RotatedRect rect, cv::Point2f anglePoint)
{
	Marker::id = id;
	Marker::rect = rect;
	Marker::angle = computeAngle(anglePoint, rect.center);
	motionCenterVec = cv::Point2f(0,0);
}

Marker::Marker()
{
	Marker::id = -1;
}

int Marker::isTracked()
{
	return isTrack;
}

int Marker::isVisible()
{
	return Marker::isVis;
}

cv::Point2f Marker::getMotionCenterVec()
{
	return motionCenterVec;
}

void Marker::clear() {
	
	isVis = 1;
	arucoID = 0;
	id=-1;
	angle = 0.;
	rect = cv::RotatedRect();
	motionCenterVec = cv::Point2f();
	int isTrack = 0;
}

int Marker::getId()
{
	return id;
}

int Marker::getArucoID()
{
	return arucoID;
}

cv::RotatedRect Marker::getRect()
{
	return rect;
}

int* Marker::getIdPointer()
{
	return &id;
}

std::vector<cv::Point2f> Marker::getPoints()
{
	std::vector<cv::Point2f> rectPoints;
	cv::Point2f p[4];
	rect.points(p);
	for (int i = 0; i < 4; i++)
	{
		rectPoints.push_back(p[i]);
	}
	return rectPoints;
}

cv::Point2f Marker::getCenter()
{
	return rect.center;
}

float Marker::getAngle()
{
	return angle;
}

std::vector<cv::Point3d> Marker::getTvecs()
{
	return tvecs;
}



cv::Point3d Marker::getEstimatedCenter()
{
	return estimatedCenter;
}

void Marker::setEstimatedCenter(cv::Point3d estiCenter)
{
	Marker::estimatedCenter = estiCenter;

}

void Marker::setTvecs(std::vector<cv::Point3d> tvecs)
{
	Marker::tvecs = tvecs;
}


void Marker::setAngle(float angle) {
	Marker::angle = angle;
}
void Marker::setTracked(int isTracked)
{
	Marker::isTrack = isTracked;
}

void Marker::setVisible(int isVisible)
{
	Marker::isVis = isVisible;
}

void Marker::setMotionCenterVec(cv::Point2f motionCenterVec)
{
	Marker::motionCenterVec = motionCenterVec;
}

void Marker::setRect(cv::RotatedRect rect)
{
	Marker::rect = rect;
	
}
void Marker::setRectWithAngle(cv::RotatedRect rect, cv::Point2f anglePoint)
{
	Marker::rect = rect;
	Marker::angle = computeAngle(anglePoint, rect.center);

	}
void Marker::setId(int id)
{
	Marker::id = id;
}

void Marker::setArucoID(int arucoID)
{
	Marker::arucoID = arucoID;
}

