// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elecappliance.h"
#include "Battery.generated.h"

enum class FCircuitType
{
	Series,
	Parallel
};

struct FElecTree
{
	TArray<FElecTree*> Childs;
	FCircuitType CircuitType;
};

/**
 * 
 */
UCLASS()
class PHYSICSLABUNREAL_API ABattery : public AElecappliance
{
	GENERATED_BODY()

	void Internal_Electrify(FElecLinkInfo BeginSearch, TArray<TArray<AElecappliance*>>& ElecPaths, TArray<AElecappliance*>& SearchMap);
	void Internal_GenElecTree();
public:
	
	//Battery will use It own Voltage instead of the Voltage parameter
	virtual void Electrify_Implementation(float Voltage) override;
};
