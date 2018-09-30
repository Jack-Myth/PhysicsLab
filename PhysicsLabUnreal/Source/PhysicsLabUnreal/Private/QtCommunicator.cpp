// Fill out your copyright notice in the Description page of Project Settings.

#include "QtCommunicator.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "CommandLine.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"

void UQtCommunicator::TryConnect()
{
	FTcpSocketBuilder tcpBuilder("Make Communication with QtClient");
	QtCommunicator= tcpBuilder.AsNonBlocking().AsReusable().Build();
	TSharedRef<FInternetAddr> LocalQtAddr= ISocketSubsystem::Get()->CreateInternetAddr(FIPv4Address(127,0,0,1).Value, 10703);
	if (QtCommunicator->Connect(LocalQtAddr.Get()))
	{
		//QtCommunicator->has
	}
}
