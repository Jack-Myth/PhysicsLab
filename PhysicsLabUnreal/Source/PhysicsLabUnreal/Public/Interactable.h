// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UENUM(BlueprintType)
enum class EQtPropertyType :uint8
{
	QPT_Float	UMETA(DisplayName="Float"),
	QPT_String	UMETA(DisplayName="String"),
	QPT_UserDefined	UMETA(DisplayName = "UserDefined"),
	QPT_Unknow	UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FQtPropertyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString DisplayName;
	UPROPERTY(BlueprintReadWrite)
		EQtPropertyType Type;
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
		void OnActorSelected(UObject* ObjBasePtr, AQtCommunicator* QtCommunicatorC);
	virtual void OnActorSelected_Implementation(UObject* ObjBasePtr, AQtCommunicator* QtCommunicatorC);

	UFUNCTION(BlueprintNativeEvent)
		void OnActorDeselected(UObject* ObjBasePtr, AQtCommunicator* QtCommunicatorC);
	virtual void OnActorDeselected_Implementation(UObject* ObjBasePtr, AQtCommunicator* QtCommunicatorC);

	UFUNCTION(BlueprintNativeEvent)
		TMap<FString,FQtPropertyInfo> CollectSyncableProperty();

	UFUNCTION(BlueprintNativeEvent)
		void OnPropertyValueChanged(const FString& PropertyName,const FString& ValueStr);
};
