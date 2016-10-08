#pragma once
#include <opencv2/opencv.hpp>

class Marker 
{
private:
		unsigned char id; //0...255
		unsigned char markCornerID;
		std::vector<cv::Point2f> rectPoints;
		cv::Point2f center;
		float angle;
		Marker(const Marker& mark) : id(mark.id),rectPoints(mark.rectPoints),markCornerID(mark.markCornerID),center(mark.center),angle(mark.angle) {};
		Marker& operator=(const Marker& mark) {
			id = mark.id;
			rectPoints = mark.rectPoints;
			markCornerID = mark.markCornerID;
			center = mark.center;
			angle = mark.angle;
			return *this;
		}
		float computeAngle(unsigned char markCornerID, std::vector<cv::Point2f> rectPoints);

public:
	Marker(unsigned char id, cv::RotatedRect rectPoints, unsigned char markCornerID);
	~Marker();
	//getter
	unsigned char getId();
	std::vector<cv::Point2f> getPoints();
	unsigned char getMarkedCornerID();
	cv::Point2f getCenter();
	float getAngle();

	//setter
	void setPoints(cv::RotatedRect points);
	void setMarkedCornerID(unsigned char id);
	void setAngle(float angle);

};

