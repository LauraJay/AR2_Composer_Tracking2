#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>
#include <PlaneCalibration.h>


class TCP_output {

private:
	cv::RotatedRect normalizeCoord(cv::RotatedRect r, cv::Size size);
	PlaneCalibration::planeCalibData pcd;

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
		int isVisible;
	};
	int startTCPServer();
	void sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec);
	void getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec);
	TCP_output(PlaneCalibration::planeCalibData pcd);
	~TCP_output();

};