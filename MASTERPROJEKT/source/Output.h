#pragma once
#include <opencv2/opencv.hpp>
#include <Marker.h>


class Output {

private:


public:
	//void TCPsendData(std::vector<Marker*> allMarkers);
	int startTCPServer(std::vector<Marker*> allMarkers);
	Output();
	~Output();

};