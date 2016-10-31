#pragma once
#include <opencv2/opencv.hpp>

class Marker 
{
private:
		int id; 
		unsigned char markCornerID;
		std::vector<cv::Point2f> rectPoints;
		cv::Point2f center;
		float angle;
		cv::Point2f motionCenterVec;
		cv::Point2f motionMarkCornerVec;

		Marker(const Marker& mark) : id(mark.id),rectPoints(mark.rectPoints),markCornerID(mark.markCornerID),center(mark.center),angle(mark.angle),motionCenterVec(mark.motionCenterVec),motionMarkCornerVec(mark.motionMarkCornerVec) {};
		Marker& operator=(const Marker& mark) {
			id = mark.id;
			rectPoints = mark.rectPoints;
			markCornerID = mark.markCornerID;
			center = mark.center;
			angle = mark.angle;
			motionCenterVec = mark.motionCenterVec;
			motionMarkCornerVec=mark.motionMarkCornerVec;
			return *this;
		}
		

public:
	Marker(int id, std::vector<cv::Point2f> rect, cv::Point2f center, unsigned char markCornerID);
	Marker();
	~Marker();
	float computeAngle(unsigned char markCornerID, std::vector<cv::Point2f> rectPoints);

	//getter
	cv::Point2f getMotionCenterVec();
	cv::Point2f getMotionMarkCornerVec();
	int getId();
	int* getIdPointer();
	std::vector<cv::Point2f> getPoints();
	unsigned char getMarkedCornerID();
	cv::Point2f getCenter();
	float getAngle();

	//setter
	void setMotionCenterVec(cv::Point2f motionCenterVec);
	void setMotionMarkCornerVec(cv::Point2f motionMarkCornerVec);
	void setPoints(std::vector<cv::Point2f>  rect, cv::Point2f center);
	void setMarkedCornerID(int id);
	void setAngle(float angle);
	void setId(int id);
};

