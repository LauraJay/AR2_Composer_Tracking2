#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>


class Output {

private:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
	};

public:

	int startTCPServer();
	void sendTCPData(std::vector<Marker*> allMarkers);
	const char* getPointerOfMarkerVec(std::vector<Marker*> allMarkers);
	Output();
	~Output();

};