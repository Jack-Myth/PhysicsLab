// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AllowWindowsPlatformTypes.h"
#include <windows.h>
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
	static WNDPROC OriginalWndProc;
	static LRESULT ModifiedWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	AActor* SelectedActor=nullptr;
	TSharedPtr<FJsonValue> Internal_CollectActorChild(AActor* TargetActor,TArray<AActor*>& SceneActorCollectionRef);
public:
	UFUNCTION(BlueprintCallable)
		void TryConnect();
	UFUNCTION(BlueprintCallable)
		void SyncSceneToQt();
	UFUNCTION(BlueprintCallable)
		void SyncActorDetails(AActor* TargetActor);
	void SendMsg(const TArray<char>& Data);
	void SendJson(const FJsonObject& JsonToSend);

	//Communicate Action
	UFUNCTION()
		void RequestHwnd();
	UFUNCTION()
		void SelectActor();
	UFUNCTION()
		void Quit();
	UFUNCTION()
		void RequestRefresh();
	UFUNCTION()
		void SendActorDetail();
};
