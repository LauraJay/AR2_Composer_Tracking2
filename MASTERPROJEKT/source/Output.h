#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>


class Output {

private:


public:

	int startTCPServer();
	void sendTCPData(std::vector<Marker*> allMarkers);
	Output();
	~Output();

};