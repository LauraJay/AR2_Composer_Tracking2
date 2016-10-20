#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>


class Output {

private:
	

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
	};
	int startTCPServer();
	void sendTCPData(std::vector<Marker*> allMarkers);
	void getPointerOfMarkerVec(std::vector<Marker*> allMarkers);
	Output();
	~Output();

};