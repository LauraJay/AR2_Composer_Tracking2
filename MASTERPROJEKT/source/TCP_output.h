#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>


class TCP_output {

private:
	

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
	};
	int startTCPServer();
	void sendTCPData(std::array<Marker*, 200> allMarkers, std::vector<int> takenIdVec);
	void getPointerOfMarkerVec(std::array<Marker*, 200>  allMarkers, std::vector<int> takenIdVec);
	TCP_output();
	~TCP_output();

};