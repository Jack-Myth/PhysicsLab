// Fill out your copyright notice in the Description page of Project Settings.

#include "QtCommunicator.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "CommandLine.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "JsonReader.h"
#include "JsonObject.h"
#include "JsonSerializer.h"
#include "Engine/GameViewportClient.h"
#include "SWindow.h"
#include "GenericWindow.h"
#include "Engine/Engine.h"
#include "d:/GitHub/UnrealEngine4.20/Engine/Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void AQtCommunicator::TryConnect()
{
	FTcpSocketBuilder tcpBuilder("Make Communication with QtClient");
	QtCommunicator= tcpBuilder.AsNonBlocking().AsReusable().Build();
	TSharedRef<FInternetAddr> LocalQtAddr= ISocketSubsystem::Get()->CreateInternetAddr(FIPv4Address(127,0,0,1).Value, 10703);
	if (QtCommunicator->Connect(LocalQtAddr.Get()))
	{
		GetWorld()->GetTimerManager().SetTimer(CommunicatorTimerHandler, this, &AQtCommunicator::CheckPendingMsgData, 0.016f, true);
	}
}

void AQtCommunicator::SendMsg(const TArray<char>& Data)
{
	int DataSize=Data.Num();
	TArray<char> TargetData;
	TargetData.SetNum(DataSize + sizeof(int),false);
	FMemory::Memcpy(TargetData.GetData(), &DataSize, sizeof(int));
	FMemory::Memcpy(TargetData.GetData() + sizeof(int), Data.GetData(), DataSize);
	int byteSent;
	QtCommunicator->Send((uint8*)TargetData.GetData(), TargetData.Num(), byteSent);
}

void AQtCommunicator::RequestHwnd()
{
	void* Hwnd = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	FJsonObject newJson;
	newJson.SetStringField("Action", "SendHwnd");
	newJson.SetStringField("Hwnd", FString::Printf(TEXT("%lld"), (long long)Hwnd));
	FString JsonStr;
	auto JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonStr, 0);
	FJsonSerializer::Serialize(MakeShared<FJsonObject>(newJson), JsonWriter);
	JsonStr = JsonStr.Replace(TEXT("\t"), TEXT(""));
	TArray<char> DataCollection;
	char* pC = TCHAR_TO_ANSI(*JsonStr);
	while(*pC)
	{
		DataCollection.Push(*pC);
		pC++;
	}
	DataCollection.Push(0);
	SendMsg(DataCollection);
}

void AQtCommunicator::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit);
}

void AQtCommunicator::InvokeAction()
{
	auto JsonReader = TJsonReaderFactory<TCHAR>::Create(FString(CachedMessage.CacheMessageData.GetData()));
	TSharedPtr<FJsonObject> JsonParsed = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(JsonReader, JsonParsed);
	TargetMsg = JsonParsed.Get();
	FString ActionName = JsonParsed->GetStringField("Action");
	UFunction* ActionTarget = this->FindFunction(*ActionName);
	if (ActionTarget)
	{
		this->ProcessEvent(ActionTarget, nullptr);
	}
	TargetMsg = nullptr;
}

void AQtCommunicator::CheckPendingMsgData()
{
	uint32 PendingDataSize;
	if (QtCommunicator->HasPendingData(PendingDataSize))
	{
		if (CachedMessage.TargetLength)
		{
			int bufferRead=0;
			QtCommunicator->Recv((uint8*)CachedMessage.CacheMessageData.GetData() + CachedMessage.CurrentLength,
				CachedMessage.TargetLength - CachedMessage.CurrentLength, bufferRead);
			CachedMessage.CurrentLength += bufferRead;
		}
		else if (PendingDataSize > sizeof(int))
		{
			QtCommunicator->Recv((uint8*)&CachedMessage.TargetLength, sizeof(int), CachedMessage.CurrentLength);
			CachedMessage.CacheMessageData.SetNum(CachedMessage.TargetLength, false);
			QtCommunicator->Recv((uint8*)CachedMessage.CacheMessageData.GetData(), CachedMessage.CacheMessageData.Num(), CachedMessage.CurrentLength);
		}
		if (CachedMessage.CurrentLength==CachedMessage.TargetLength&&CachedMessage.TargetLength>0)
		{
			InvokeAction();
			CachedMessage.CacheMessageData.Empty();
			CachedMessage.CurrentLength = 0;
			CachedMessage.TargetLength = 0;
		}
	}
}
