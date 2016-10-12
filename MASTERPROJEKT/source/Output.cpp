#include "Output.h"
#pragma comment(lib,"ws2_32.lib") //ggf. auskommentieren
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

Output::Output()
{

}

Output::~Output()
{

}

void Output::startTCPServer()
{
	Output* w = new Output();
	int winsock = w->startWinsock();
	
}

//Prototypen
int startWinsock2(void);

int Output::startWinsock()
{
	long rc;
	rc = startWinsock2();
	if (rc != 0)
	{
		printf("Fehler: startWinsock, fehler code: %d\n", rc);
		return 1;
	}
	else
	{
		printf("Winsock gestartet!\n");
	}
	return 0;
}

int startWinsock2(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}