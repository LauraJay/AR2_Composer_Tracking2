#pragma once
#include <opencv2/opencv.hpp>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>


class MarkerSizeCalibration {

private:

public:
	~MarkerSizeCalibration() {};
	MarkerSizeCalibration() {};
	int runMarkerSizeCalibration();
};