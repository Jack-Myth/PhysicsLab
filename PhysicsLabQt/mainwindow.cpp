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
        UnrealCommunicatorHelper::SendJson(UnrealCommunicator,JsonD.toBinaryData().data(),JsonD.toBinaryData().length());
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
    QJsonDocument MessageJson = QJsonDocument::fromBinaryData(QByteArray::fromRawData(
                                                                  CachedMessage.CachedMessageData.data(),
                                                                  CachedMessage.CachedMessageData.length()));
    if (MessageJson.isNull())
        return;
    QJsonDocument* JsonDocPtr=&MessageJson;
    QJsonObject MessageObj = MessageJson.object();
    QGenericArgument Jsonptr("JsonPtr",&JsonDocPtr);
    this->metaObject()->invokeMethod(this,MessageObj.find("Action").value().toString().toStdString().c_str(),Jsonptr);
}

void MainWindow::SendHwnd(QJsonDocument *JsonPtr)
{
    if(Splash::self)
        Splash::self->close();
    HWND UnrealHwnd = (HWND)JsonPtr->object().find("Hwnd").value().toString().toULong();
    SetParent(UnrealHwnd,(HWND)ui->UnrealFrame->winId());
}
