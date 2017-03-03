#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>
#include <PlaneAndAffineCalibration.h>


class TCP {

private:

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float posZ;
		float angle;
		int isVisible;
	};


	enum TCPstatus { planeAndPoseCalib, planeOnlyCalib, sceneStart, PlaneCalibDone,PoseCalibDone, ControlerButtonPressed, ArucoFound, ArucoNotFound, reCalib};




	struct Calibration {
		int isCalibrated;
	};

	int startTCPServer();
	void sendMarkerData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec,cv::Mat frame);
	void sendStatus(int status);
	int receiveStatus();
	cv::Point3f receiveControllerPositions();
	void getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec,cv::Mat frame);
	TCP(cv::Size frameSize);
	~TCP();
};
