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
		//mvC
		//mvMC
		Marker(const Marker& mark) : id(mark.id),rectPoints(mark.rectPoints),markCornerID(mark.markCornerID),center(mark.center),angle(mark.angle) {};
		Marker& operator=(const Marker& mark) {
			id = mark.id;
			rectPoints = mark.rectPoints;
			markCornerID = mark.markCornerID;
			center = mark.center;
			angle = mark.angle;
			//mvC
			//mvMC
			return *this;
		}
		

public:
	Marker(int id, std::vector<cv::Point2f> rect, cv::Point2f center, unsigned char markCornerID);
	~Marker();
	float computeAngle(unsigned char markCornerID, std::vector<cv::Point2f> rectPoints);

	//getter
	int getId();
	int* getIdPointer();
	std::vector<cv::Point2f> getPoints();
	unsigned char getMarkedCornerID();
	cv::Point2f getCenter();
	float getAngle();

	//setter
	void setPoints(std::vector<cv::Point2f>  rect, cv::Point2f center);
	void setMarkedCornerID(int id);
	void setAngle(float angle);
	void setId(int id);

	

};

