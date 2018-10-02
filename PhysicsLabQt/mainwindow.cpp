#include "mainwindow.h"
#include "splash.h"
#include "ui_mainwindow.h"
#include "unrealcommunicatorhelper.h"
#include <QtNetwork>
#include <Windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Splash::self=new Splash();
    Splash::self->show();
    CachedMessage.CachedMessageTargetSize=0;
    UnrealCommunicatorServer.listen(QHostAddress::Any,10703);
    ui->UnrealFrame->setAttribute(Qt::WA_NativeWindow);
    connect(&UnrealCommunicatorServer,&QTcpServer::newConnection,this,&MainWindow::OnUnrealConnected,Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnUnrealConnected()
{
    if (!UnrealCommunicator)
    {
        UnrealCommunicator=UnrealCommunicatorServer.nextPendingConnection();
        connect(UnrealCommunicator,&QTcpSocket::readyRead,this,&MainWindow::OnCommunicationMessageArrived,Qt::UniqueConnection);
        UnrealCommunicatorServer.close();
        QJsonDocument JsonD;
        QJsonObject JsonO;
        JsonO.insert("Action","RequestHwnd");
        JsonD.setObject(JsonO);
        UnrealCommunicatorHelper::SendJson(UnrealCommunicator,JsonD.toJson().data(),JsonD.toBinaryData().length());
    }
}

void MainWindow::OnCommunicationMessageArrived()
{
    if(!CachedMessage.CachedMessageTargetSize)
    {
        int PackageSize;
        UnrealCommunicator->read((char*)&PackageSize,sizeof(int));
        CachedMessage.CachedMessageTargetSize=PackageSize;
        CachedMessage.CachedMessageData.resize(CachedMessage.CachedMessageTargetSize);
        CachedMessage.CachedMessageCurrentSize = UnrealCommunicator->read(CachedMessage.CachedMessageData.data(),
                                                                          CachedMessage.CachedMessageTargetSize);
        if(CachedMessage.CachedMessageCurrentSize>=CachedMessage.CachedMessageTargetSize)
        {
            InvokeAction();
            CachedMessage.CachedMessageData.clear();
            CachedMessage.CachedMessageCurrentSize=0;
            CachedMessage.CachedMessageTargetSize=0;
        }
    }
    else
    {
        CachedMessage.CachedMessageCurrentSize += UnrealCommunicator->read(
                    CachedMessage.CachedMessageData.data()+CachedMessage.CachedMessageCurrentSize,
                    CachedMessage.CachedMessageTargetSize-CachedMessage.CachedMessageCurrentSize);
        if (CachedMessage.CachedMessageCurrentSize>=CachedMessage.CachedMessageTargetSize)
        {
            InvokeAction();
            CachedMessage.CachedMessageData.clear();
            CachedMessage.CachedMessageCurrentSize=0;
            CachedMessage.CachedMessageTargetSize=0;
        }
    }
}

void MainWindow::InvokeAction()
{
    QJsonParseError ErrorMsg;
    QString JsonStr=QString(CachedMessage.CachedMessageData.data());
    QByteArray JsonData=JsonStr.toUtf8();
    QJsonDocument MessageJson = QJsonDocument::fromJson(JsonData,&ErrorMsg);
    if(ErrorMsg.error!=QJsonParseError::NoError)
    {
        qDebug()<<ErrorMsg.errorString();
    }
    if (MessageJson.isNull())
        return;
    TargetMsg=&MessageJson;
    QJsonObject MessageObj = MessageJson.object();
    this->metaObject()->invokeMethod(this,MessageObj.find("Action").value().toString().toStdString().c_str());
}

void MainWindow::SendHwnd()
{
    if(Splash::self)
        Splash::self->close();
    HWND UnrealHwnd = (HWND)TargetMsg->object().find("Hwnd").value().toString().toLongLong();
    SetParent(UnrealHwnd,(HWND)ui->UnrealFrame->winId());
    SetWindowPos(UnrealHwnd,NULL,0,0,ui->UnrealFrame->size().width(),ui->UnrealFrame->size().height(),NULL);
    this->show();
}
