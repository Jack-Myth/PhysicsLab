// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

USTRUCT(BlueprintType)
struct FQtPropertyInfo
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString DisplayName;
	UPROPERTY(BlueprintReadWrite)
		FString Type;
	UPROPERTY(BlueprintReadWrite)
		FString ValueStr;
};

// Base UClass of IInteractable.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PHYSICSLABUNREAL_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		void OnActorSelected();

	UFUNCTION(BlueprintNativeEvent)
		void OnActorDeselected();

	UFUNCTION(BlueprintNativeEvent)
		TMap<FString,FQtPropertyInfo> CollectSyncableProperty();

	UFUNCTION(BlueprintNativeEvent)
		void OnPropertyValueChanged(const FString& PropertyName,const FString& ValueStr);
};
