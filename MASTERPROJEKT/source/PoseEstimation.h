#pragma once
#include <opencv2/opencv.hpp>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>


class PoseEstimation {

private:

public:
	~PoseEstimation() {};
	PoseEstimation() {};
	int runPoseEstimation();
};