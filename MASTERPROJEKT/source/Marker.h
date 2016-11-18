#pragma once
#include <opencv2/opencv.hpp>

class Marker 
{
private:
		int id,arucoID; 
		int isVis = 1;
		cv::RotatedRect rect;
		float angle;
		cv::Point2f motionCenterVec;
		int isTrack=0;
		
public:
	Marker(int id, cv::RotatedRect rect, cv::Point2f anglePoint);
	Marker();
	~Marker() {};
	float computeAngle(cv::Point2f angelPoint, cv::Point2f center);

	//getter
	int isTracked();
	int isVisible();
	cv::Point2f getMotionCenterVec();
	int getId();
	int getArucoID();
	cv::RotatedRect getRect();
	int* getIdPointer();
	std::vector<cv::Point2f> getPoints();
	cv::Point2f getCenter();
	float getAngle();


	//setter
	void setAngle(float angle);
	void setTracked(int isTracked);
	void setVisible(int isVisible);
	void setMotionCenterVec(cv::Point2f motionCenterVec);
	void setRectWithAngle(cv::RotatedRect rect, cv::Point2f anglePoint);
	void setRect(cv::RotatedRect rect);
	void setId(int id);
	void setArucoID(int arucoID);

	void clear();
	
};

