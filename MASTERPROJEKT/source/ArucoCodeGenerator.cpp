#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
//#define inUse
using namespace cv;
// ATTENTION!!!! Comment main() in Main before compiling
#ifdef inUse
int main() {
	int borderBits = 1; // Breite des Rands
	int markerSize = 500; // Ausgabebildgröße
	std::string out = "ARUCO_ID_ORIGINAL_";
	int nMarkerToPrint = 1;
	Ptr <aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_ARUCO_ORIGINAL);

	// evtl. von 1 an
	for (int i = 1; i <= nMarkerToPrint; i++)
	{

	Mat markerImg;
	aruco::drawMarker(dictionary,800,markerSize, markerImg, borderBits);

	if (!markerImg.empty()) {
		
		out += std::to_string(800);
		imshow(out, markerImg);
		waitKey(1);
		out += ".png";
	imwrite(out, markerImg);
	out = "ARUCO_ID_ORIGINAL_";
	}
	}
		return 0;
}

#endif // inUse