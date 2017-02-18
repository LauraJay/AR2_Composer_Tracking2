#include "TCP.h"
//#pragma comment(lib,"ws2_32.lib") //ggf. auskommentieren
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

int startWinsock(void);
long rc;
SOCKET serverSocket;
SOCKET connectedSocket;
int c;
std::ofstream myfile;
std::ofstream myfile2;
struct TCP::MarkerStruct ms[101];
struct TCP::Calibration m[1];

int TCP::startTCPServer()
{
	SOCKADDR_IN addr;
	// start Winsock
	rc = startWinsock();
	if (rc != 0) {
		printf("ERROR: startWinsock, code: %d\n", rc);
		return 1;
	}
	else {
		printf("Winsock started!\n");
	}
	// build Socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		printf("ERROR: The Socket could not be build, code: %d\n", WSAGetLastError());
		return 1;
	}
	else {
		printf("Socket built!\n");
	}

	//define port
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	addr.sin_addr.s_addr = ADDR_ANY;
	rc = bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
	if (rc == SOCKET_ERROR) {
		printf("ERROR: bind, code: %d\n", WSAGetLastError());
		return 1;
	}
	else {
		printf("port nr. is set to 10000\n");
	}
	// set server Socket in listen modus 
	rc = listen(serverSocket, 10);
	if (rc == SOCKET_ERROR) {
		printf("ERROR: listen, code: %d\n", WSAGetLastError());
		return 1;
	}
	else {
		printf("server Socket is set to listen....\n");
	}

	//accept connection
	connectedSocket = accept(serverSocket, NULL, NULL);
	if (connectedSocket == INVALID_SOCKET)
	{
		printf("EROOR: accept, code: %d\n", WSAGetLastError());
		return 1;
	}
	else {
		printf("New connection accepted!\n");
	}
}

int startWinsock(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}

void TCP::sendStatus(int status) {
	const char far* markerPointer = (const char*)&status;
	send(connectedSocket, markerPointer, 4, 0);
	printf("Sent Status: %d \n", status);

}

void TCP::sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec, cv::Mat frame) {
	getPointerOfMarkerVec(allMarkers, takenIdVec, frame);
	const char far* markerPointer = (const char*)&ms;
	send(connectedSocket, markerPointer, 2004, 0);
	//printf("Data sent...\n");

	/*const char FAR* markerPointer = (const char*) &allMarkers;
	rc = send(connectedSocket, markerPointer, 4100, 0);*/
}

int TCP::receiveTCPData() {
	char far* mPointer = (char*)&m;
	recv(connectedSocket, mPointer, 4, 0);
	printf("Received Status: %i \n", m[0].isCalibrated);
	return m[0].isCalibrated;
}

void TCP::getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec, cv::Mat frame) {

	myfile.open("logNorm.txt", std::ios::out | std::ios::app);
	myfile << "Current Frame " << c << "\n";
	c++;
	myfile << "\t allMarkersSize() " << allMarkers.size() << "\n";

	for (int i = 0; i < allMarkers.size(); i++) {
		ms[i].id = allMarkers[i]->getId();
	
		if (allMarkers[i]->getId() > 0)myfile << "\t tid " << ms[i].id << "\n";
		//// For Simple Normalisation
		cv::RotatedRect r = normalizeCoord(allMarkers[i]->getRect());
		ms[i].posX = r.center.x;
		ms[i].posY = r.center.y;
		
		if (allMarkers[i]->getId() > 0) {
			myfile << "\t posX " << r.center.x << "\n";
			myfile << "\t posY " << r.center.y << "\n";
		}
				
		ms[i].angle = allMarkers[i]->getAngle();
		if (allMarkers[i]->getId() > 0)myfile << "\t angle " << ms[i].angle << "\n";
		ms[i].isVisible = allMarkers[i]->isVisible();
		if (allMarkers[i]->getId() > 0)myfile << "\t isVisible " << ms[i].isVisible << "\n";
	}

	ms[allMarkers.size()].id = -2;

	myfile.close();
	myfile2.close();
}


TCP::TCP(cv::Size frameSize) {
	//lut = cv::Mat3f(frameSize);
	//readCameraParameters("LUT.yml", lut);
	myfile.open("logNorm.txt", std::ofstream::out | std::ofstream::trunc);
	myfile.close();
	myfile2.open("logWorld.txt", std::ofstream::out | std::ofstream::trunc);
	myfile2.close();
}

TCP::~TCP() {
}

//int TCP::loadLUT()
//{
//	readCameraParameters("LUT.yml", lut);
//	return 0;
//}

void TCP::setPCD(PlaneCalibration::planeCalibData pcData) {
	pcd = pcData;
}

cv::RotatedRect TCP::normalizeCoord(cv::RotatedRect r) {
	cv::Point2f center = r.center;
	//printf("Before coord: %f, %f; ", center.x, center.y);
	center.x = abs((center.x - pcd.lowerCorner.x) / pcd.size.width);
	center.y = abs((center.y- pcd.upperCorner.y) / pcd.size.height);
	//printf("Normalized coord: %f, %f; ", center.x, center.y);
	cv::Size2f normSize = cv::Size2f((r.size.width / pcd.size.width), (r.size.height / pcd.size.height));
	cv::RotatedRect rect = cv::RotatedRect(center, normSize, r.angle);
	return rect;
}


bool TCP::readCameraParameters(std::string filename, cv::Mat3f &lut) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["LUT"] >> lut;
	return true;
}

cv::Vec3f TCP::computeCamera2World(cv::Point2f point)
{
	cv::Vec3f worldCenter = cv::Vec3f();
	worldCenter = lut.at<cv::Vec3f>(point.y,point.x);
	worldCenter[2] = 0.;
	return worldCenter;
}
