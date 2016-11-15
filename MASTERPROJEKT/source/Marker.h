#pragma once
#include <opencv2/opencv.hpp>

class Marker 
{
private:
		int id,arucoID; 
		int isVis = 1;
		cv::RotatedRect rect;
		cv::Point2f motionCenterVec;
		int isTrack=0;
		
public:
	Marker(int id, cv::RotatedRect rect);
	Marker();
	~Marker() {};
	float computeAngle(unsigned char markCornerID, std::vector<cv::Point2f> rectPoints);

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
	void setTracked(int isTracked);
	void setVisible(int isVisible);
	void setMotionCenterVec(cv::Point2f motionCenterVec);
	void setRect(cv::RotatedRect rect);
	void setId(int id);
	void setArucoID(int arucoID);
	
};

