// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QtCommunicator.generated.h"

/**
 * 
 */
UCLASS()
class UQtCommunicator : public UObject
{
	GENERATED_BODY()

	class FSocket* QtCommunicator;
public:
	void TryConnect();
	
};
