#include "unrealcommunicatorhelper.h"

#include <QTcpSocket>

void UnrealCommunicatorHelper::SendJson(QTcpSocket *UnrealCommunicator, char *pData, int length)
{
    QVector<char> DataToSend;
    DataToSend.resize(length+sizeof(int));  //Of course,it will be 4.
    memcpy_s(DataToSend.data(),sizeof(int),(void*)&length,sizeof(int));
    memcpy_s(DataToSend.data()+sizeof(int),length,pData,length);
    UnrealCommunicator->write(DataToSend.data(),DataToSend.length());
}
