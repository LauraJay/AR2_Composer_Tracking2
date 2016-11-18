#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>


class TCP_output {

private:
	cv::RotatedRect normalizeCoord(cv::RotatedRect r, cv::Size size);

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
	};
	int startTCPServer();
	void sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec);
	void getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec);
	TCP_output();
	~TCP_output();

};