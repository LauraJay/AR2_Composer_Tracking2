//#pragma once
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/aruco.hpp>
//
//using namespace cv;
//
//int main(int argc, char *argv[]) {
//
//	int dictionaryId = aruco::DICT_4X4_50; // Anzahl der Bits und Max Anz. der IDS
//	int borderBits = 1; // Breite des Rands
//	int markerSize = 500; // Ausgabebildgröße
//
//	String out = "ARUCO_ID";
//	int nMarkerToPrint = 4;
//	
//
//	aruco::Dictionary* dictionary =	aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
//
//	// evtl. von 1 an
//	for (int i = 0; i < nMarkerToPrint; i++)
//	{
//
//	Mat markerImg;
//	aruco::drawMarker(dictionary, i, markerSize, markerImg, borderBits);
//
//	if (!markerImg.empty()) {
//		imshow("marker", markerImg);
//		waitKey(0);
//		out+=(""+i);
//		out += ".png";
//	imwrite(out, markerImg);
//	out = "ARUCO_ID";
//	}
//	}
//		return 0;
//}