#include "TCP.h"
#pragma comment(lib,"ws2_32.lib") //ggf. auskommentieren
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
struct TCP::MarkerStruct ms[101];
struct TCP::Calibration m[1];

int TCP::startTCPServer()
{
	SOCKADDR_IN addr; 
	// start Winsock
	rc = startWinsock();
	if (rc != 0){
		printf("ERROR: startWinsock, code: %d\n", rc);
		return 1;
	}else{
		printf("Winsock started!\n");
	} 
	// build Socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET){
		printf("ERROR: The Socket could not be build, code: %d\n", WSAGetLastError());
		return 1;
	}
	else{
		printf("Socket built!\n");
	} 
	
	//define port
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	addr.sin_addr.s_addr = ADDR_ANY;
	rc = bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
	if (rc == SOCKET_ERROR){
		printf("ERROR: bind, code: %d\n", WSAGetLastError());
		return 1;
	}
	else{
		printf("port nr. is set to 10000\n");
	}
	// set server Socket in listen modus 
	rc = listen(serverSocket, 10);
	if (rc == SOCKET_ERROR){
		printf("ERROR: listen, code: %d\n", WSAGetLastError());
		return 1;
	}
	else{
		printf("server Socket is set to listen....\n");
	}

	//accept connection
	connectedSocket = accept(serverSocket, NULL, NULL);
	if (connectedSocket == INVALID_SOCKET)
	{
		printf("EROOR: accept, code: %d\n", WSAGetLastError());
		return 1;
	}
	else{
		printf("New connection accepted!\n");
	}
}

int startWinsock(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}

void TCP::sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec) {
	getPointerOfMarkerVec(allMarkers, takenIdVec);
	const char far* markerPointer = (const char*)&ms;
	send(connectedSocket, markerPointer, 2004, 0);
	/*const char FAR* markerPointer = (const char*) &allMarkers;
	rc = send(connectedSocket, markerPointer, 4100, 0);*/
}

int TCP::receiveTCPData() {
	char far* mPointer = (char*)&m;
	recv(connectedSocket, mPointer, 4, 0 );	
	printf("%i ", m[0].isCalibrated);
	return m[0].isCalibrated;
}

void TCP::getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec) {
	
	myfile.open("log.txt", std::ios::out | std::ios::app);
	myfile << "Current Frame " << c << "\n";
	c++;
		myfile << "\t allMarkersSize() " << allMarkers.size() << "\n";
		for (int i = 1; i <= takenIdVec.size(); i++) {
			int id = takenIdVec[i-1];
			cv::RotatedRect r = normalizeCoord(allMarkers[id]->getRect());
			ms[i-1].id = allMarkers[id]->getId();
			myfile << "\t tid " << ms[i-1].id << "\n";
			ms[i-1].posX = r.center.x;
			myfile << "\t posX " << ms[i-1].posX << "\n";
			ms[i-1].posY = r.center.y;
			myfile << "\t posY " << ms[i-1].posY << "\n";
			ms[i-1].angle = allMarkers[id]->getAngle();
			myfile << "\t angle " << ms[i-1].angle << "\n";
			ms[i - 1].isVisible = allMarkers[id]->isVisible();
			myfile << "\t isVisible " << ms[i-1].isVisible << "\n";

		}
		
		//Last id is -1 to show the end of information per frame
		ms[takenIdVec.size()].id = -1;
		myfile.close();
}


TCP::TCP(){
}

TCP::~TCP(){
}

void TCP::setPCD(PlaneCalibration::planeCalibData pcData) {
	pcd = pcData;
}

cv::RotatedRect TCP::normalizeCoord(cv::RotatedRect r) {
	cv::Point2f center = r.center;
	printf("Before coord: %f, %f; ", center.x, center.y);
	center.x = -((pcd.upperLeftCorner.x - center.x) / pcd.size.width);
	center.y = (pcd.upperLeftCorner.y - center.y) / pcd.size.height;
	printf("Normalized coord: %f, %f; ", center.x, center.y);
	cv::Size2f normSize = cv::Size2f((r.size.width / pcd.size.width),(r.size.height / pcd.size.height));
	cv::RotatedRect rect = cv::RotatedRect(center, normSize, r.angle);
	return rect;
}