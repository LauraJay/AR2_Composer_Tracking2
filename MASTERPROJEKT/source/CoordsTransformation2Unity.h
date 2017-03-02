#pragma once
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include<PlaneCalibration.h>
#include <Marker.h>
#include <array>

class CoordsTransformtion2Untiy {

private:

public:
	cv::Mat camMatrix, distCoeffs;
	cv::Mat affineTransform= cv::Mat::zeros(3, 4, CV_64F);
	int computeTransformation2Unity(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec);
	CoordsTransformtion2Untiy() {};
	~CoordsTransformtion2Untiy() {};
};
