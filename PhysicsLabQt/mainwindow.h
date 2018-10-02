#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
namespace Ui {
class MainWindow;
}

struct CachedMessageInfo
{
    QVector<char> CachedMessageData;
    int CachedMessageCurrentSize=0;
    int CachedMessageTargetSize=0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTcpServer UnrealCommunicatorServer;
    QTcpSocket* UnrealCommunicator=nullptr;
    class QJsonDocument* TargetMsg;
    CachedMessageInfo CachedMessage;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void OnUnrealConnected();
    void OnCommunicationMessageArrived();
    void InvokeAction();

private:
    //Action Function:
    Q_INVOKABLE void SendHwnd();
};

#endif // MAINWINDOW_H
