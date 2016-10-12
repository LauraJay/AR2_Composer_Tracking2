#include "Output.h"
#pragma comment(lib,"ws2_32.lib") //ggf. auskommentieren
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>


int startWinsock(void);
int Output::startTCPServer()
{
	long rc;
	SOCKET serverSocket;
	SOCKET connectedSocket;
	SOCKADDR_IN addr; 
	// Winsock starten
	rc = startWinsock();
	if (rc != 0)
	{
		printf("Fehler: startWinsock, fehler code: %d\n", rc);
		return 1;
	}
	else
	{
		printf("Winsock gestartet!\n");
	} 
	// Socket erstellen
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("Fehler: Der Socket konnte nicht erstellt werden, fehler code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("Socket erstellt!\n");
	} 
	
	//port festlegen
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(66666);
	addr.sin_addr.s_addr = ADDR_ANY;
	rc = bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
	if (rc == SOCKET_ERROR)
	{
		printf("Fehler: bind, fehler code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("Socket an port 66666 gebunden\n");
	}
	//server Socket wartet auf Verbindung und "lauscht"
	rc = listen(serverSocket, 10);
	if (rc == SOCKET_ERROR)
	{
		printf("Fehler: listen, fehler code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("serverSocket ist im listen Modus....\n");
	}
	//Verbindung akzeptieren

	connectedSocket = accept(serverSocket, NULL, NULL);
	if (connectedSocket == INVALID_SOCKET)
	{
		printf("Fehler: accept, fehler code: %d\n", WSAGetLastError());
		return 1;
	}
	else
	{
		printf("Neue Verbindung wurde akzeptiert!\n");
	}
	return 0;
}

int startWinsock(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}


Output::Output()
{

}

Output::~Output()
{

}