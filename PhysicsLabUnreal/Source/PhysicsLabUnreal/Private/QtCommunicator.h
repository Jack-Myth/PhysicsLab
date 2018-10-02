// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QtCommunicator.generated.h"

struct CachedMessageInfo
{
	TArray<char> CacheMessageData;
	int CurrentLength;
	int TargetLength;
};

/**
 * 
 */
UCLASS()
class AQtCommunicator : public AActor
{
	GENERATED_BODY()

	class FSocket* QtCommunicator;
	CachedMessageInfo CachedMessage;
	FTimerHandle CommunicatorTimerHandler;
	FJsonObject* TargetMsg=nullptr;
	void InvokeAction();
	void CheckPendingMsgData();
public:
	UFUNCTION(BlueprintCallable)
		void TryConnect();
	void SendMsg(const TArray<char>& Data);
	UFUNCTION()
		void RequestHwnd();
};
