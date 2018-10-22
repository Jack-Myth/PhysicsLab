// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DefGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PHYSICSLABUNREAL_API ADefGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
		class AQtCommunicator* QtCommunicator=nullptr;
	
	class UStaticMeshComponent* ElecPole = nullptr;
	TMap<class AActor*,TArray<class UStaticMeshComponent*>> CableLinkedMap;
public:
	UFUNCTION(BlueprintPure)
		class AQtCommunicator* GetQtCommunicator();
	static class AQtCommunicator* GetQtCommunicator(AActor* ContextActor);

	void SubmitClickedElecPole(class UStaticMeshComponent* ElecPole);
	void BreakAllLinkToPole(class UStaticMeshComponent* ElecPole);
};
