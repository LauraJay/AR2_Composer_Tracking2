#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <Marker.h>
#include <PlaneCalibration.h>


class TCP {

private:
	cv::RotatedRect normalizeCoord(cv::RotatedRect r);
	PlaneCalibration::planeCalibData pcd;
	bool readCameraParameters(std::string filename, cv::Mat3f & lut);
	cv::Mat3f lut;
	cv::Vec3f computeCamera2World(cv::Point2f point);

public:
	struct MarkerStruct {
		int id;
		float posX;
		float posY;
		float angle;
		int isVisible;
	};

	struct Calibration {
		int isCalibrated;
	};

	int startTCPServer();
	void sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec);
	int receiveTCPData();
	void getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec);
	void setPCD(PlaneCalibration::planeCalibData pcData);
	TCP(cv::Size frameSize);
	~TCP();

};