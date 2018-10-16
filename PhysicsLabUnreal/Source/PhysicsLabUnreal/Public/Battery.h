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

	struct FElecPath
	{
		TArray<AElecappliance*> ElecPath;
		int CurIndex=0;
		TArray<int> PinCollection;
		void PushPin()
		{PinCollection.Push(CurIndex);}
		int PeekLastPin()
		{return PinCollection.Last();}
		void PopPin()
		{CurIndex = PinCollection.Pop();}
		void Next()
		{CurIndex++;}
		void Prev()
		{ CurIndex--;}
		AElecappliance* GetCurrent()
		{return ElecPath[CurIndex];}
	};
	void Internal_Electrify(FElecLinkInfo BeginSearch, TArray<FElecPath>& ElecPaths, FElecPath& SearchMap);
	void Internal_GenElecTree();
public:
	
	//Battery will use It own Voltage instead of the Voltage parameter
	virtual void Electrify_Implementation(float Voltage) override;
};
