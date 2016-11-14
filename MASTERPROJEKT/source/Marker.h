#pragma once
#include <opencv2/opencv.hpp>

class Marker 
{
private:
		int id,arucoID; 
		std::vector<cv::Point2f> rectPoints;
		cv::Point2f center;
		float angle;
		cv::Point2f motionCenterVec;

		Marker(const Marker& mark) : id(mark.id),rectPoints(mark.rectPoints),center(mark.center),angle(mark.angle),motionCenterVec(mark.motionCenterVec) {};
		Marker& operator=(const Marker& mark) {
			id = mark.id;
			rectPoints = mark.rectPoints;
			center = mark.center;
			angle = mark.angle;
			motionCenterVec = mark.motionCenterVec;
			return *this;
		}
		

public:
	Marker(int id, cv::RotatedRect normRect);
	Marker();
	~Marker() {};
	float computeAngle(unsigned char markCornerID, std::vector<cv::Point2f> rectPoints);

	//getter
	cv::Point2f getMotionCenterVec();
	int getId();
	int getArucoID();
	int* getIdPointer();
	std::vector<cv::Point2f> getPoints();
	cv::Point2f getCenter();
	float getAngle();

	//setter
	void setMotionCenterVec(cv::Point2f motionCenterVec);
	void setPoints(std::vector<cv::Point2f>  rect, cv::Point2f center);
	void setAngle(float angle);
	void setId(int id);
	void setArucoID(int arucoID);
	void setCenter(cv::Point2f center);
};

