#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
using namespace cv;
// ATTENTION!!!! Comment main() in Main before compiling
int main() {
	int borderBits = 1; // Breite des Rands
	int markerSize = 500; // Ausgabebildgröße
	String out = "ARUCO_ID";
	int nMarkerToPrint = 4;
	
	aruco::Dictionary dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
	//aruco::Dictionary* dictionary =	aruco::getPredefinedDictionary(aruco::DICT_4X4_100);

	//// evtl. von 1 an
	//for (int i = 0; i < nMarkerToPrint; i++)
	//{

	//Mat markerImg;
	//aruco::drawMarker(dictionary, i, markerSize, markerImg, borderBits);

	//if (!markerImg.empty()) {
	//	imshow("marker", markerImg);
	//	waitKey(0);
	//	out+=(""+i);
	//	out += ".png";
	//imwrite(out, markerImg);
	//out = "ARUCO_ID";
	//}
	//}
		return 0;
}