#pragma once
#include <opencv2/opencv.hpp>


class Output {

private:

	//int startWinsock();

public:

	int startTCPServer();
	Output();
	~Output();

};