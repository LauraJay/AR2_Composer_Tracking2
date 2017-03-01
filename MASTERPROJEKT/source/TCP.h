#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>
#include <PlaneCalibration.h>


class TCP {

private:
	cv::RotatedRect normalizeCoord(cv::RotatedRect r);
	PlaneCalibration::planeCalibData pcd;
	cv::Vec3f computeCamera2World(cv::Point2f point);

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
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
	void setPCD(PlaneCalibration::planeCalibData pcData);
	TCP(cv::Size frameSize);
	~TCP();
};
