#include "TCP_output.h"
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
struct TCP_output::MarkerStruct ms[201];

int TCP_output::startTCPServer()
{
	SOCKADDR_IN addr; 
	// start Winsock
	rc = startWinsock();
	if (rc != 0)
	{
		printf("ERROR: startWinsock, code: %d\n", rc);
		return 1;
	}
	else
	{
		printf("Winsock started!\n");
	} 
	// build Socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("ERROR: The Socket could not be build, code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("Socket built!\n");
	} 
	
	//define port
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	addr.sin_addr.s_addr = ADDR_ANY;
	rc = bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
	if (rc == SOCKET_ERROR)
	{
		printf("ERROR: bind, code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("port nr. is set to 10000\n");
	}
	// set server Socket in listen modus 
	rc = listen(serverSocket, 10);
	if (rc == SOCKET_ERROR)
	{
		printf("ERROR: listen, code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("server Socket is set to listen....\n");
	}

	//accept connection
	connectedSocket = accept(serverSocket, NULL, NULL);
	if (connectedSocket == INVALID_SOCKET)
	{
		printf("EROOR: accept, code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("New connection accepted!\n");
	}
}

int startWinsock(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}

void TCP_output::sendTCPData(std::array<Marker*, 100> allMarkers, std::vector<int> takenIdVec) {
	getPointerOfMarkerVec(allMarkers, takenIdVec);
	const char far* markerPointer = (const char*)&ms;
	rc = send(connectedSocket, markerPointer, 3204, 0);
	/*const char FAR* markerPointer = (const char*) &allMarkers;
	rc = send(connectedSocket, markerPointer, 4100, 0);*/
}

void TCP_output::getPointerOfMarkerVec(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec) {
	
	myfile.open("log.txt", std::ios::out | std::ios::app);
	myfile << "Current Frame " << c << "\n";
	c++;
		myfile << "\t allMarkersSize() " << allMarkers.size() << "\n";
		for (int i = 1; i <= takenIdVec.size(); i++) {
			allMarkers[i]->setRect( normalizeCoord(allMarkers[i]->getRect(),cv::Size (1024,1280)));
			ms[i-1].id = allMarkers[i]->getId();
			myfile << "\t tid " << ms[i].id << "\n";
			ms[i-1].posX = allMarkers[i]->getCenter().x;
			myfile << "\t posX " << ms[i].posX << "\n";
			ms[i-1].posY = allMarkers[i]->getCenter().y;
			myfile << "\t posY " << ms[i].posY << "\n";
			ms[i-1].angle = allMarkers[i]->getAngle();
			myfile << "\t angle " << ms[i].angle << "\n";
		}
		
		//Last id is -1 to show the end of information per frame
		ms[takenIdVec.size()].id = -1;
		myfile.close();
}


TCP_output::TCP_output()
{
}

TCP_output::~TCP_output()
{
}

cv::RotatedRect TCP_output::normalizeCoord(cv::RotatedRect r, cv::Size size) {
	cv::Point2f center = r.center;
	center.x = center.x / size.width;
	center.y = center.y / size.height;
	cv::Size2f normSize = cv::Size2f((r.size.width / size.width), (r.size.height / size.height));
	cv::RotatedRect rect = cv::RotatedRect(center, normSize, r.angle);
	return rect;
}