#ifndef UNREALCOMMUNICATORHELPER_H
#define UNREALCOMMUNICATORHELPER_H


class UnrealCommunicatorHelper
{
public:
    static void SendJson(class QTcpSocket* UnrealCommunicator,char* pData,int length);
};

#endif // UNREALCOMMUNICATORHELPER_H
