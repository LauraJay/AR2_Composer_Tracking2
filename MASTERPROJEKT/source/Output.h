#pragma once
#include <opencv2/opencv.hpp>


class Output {

private:

	int startWinsock();

public:

	void startTCPServer();
	Output();
	~Output();

};