#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>
#include <PlaneCalibration.h>


class TCP_output {

private:
	cv::RotatedRect normalizeCoord(cv::RotatedRect r);
	PlaneCalibration::planeCalibData pcd;

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
		int isVisible;
	};

	struct TCPInput {
		int isCalibrated;
	};

	int startTCPServer();
	void sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec);
	int TCP_output::receiveTCPData();
	void getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec);
	TCP_output();
	void setPCD(PlaneCalibration::planeCalibData pcData);
	~TCP_output();

};