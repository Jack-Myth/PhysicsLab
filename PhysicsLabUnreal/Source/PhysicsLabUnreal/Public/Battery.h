// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elecappliance.h"
#include "Battery.generated.h"

enum class ECircuitType
{
	Series,
	Parallel
};

struct FElecTree
{
	TArray<FElecTree*> Childs;
	AElecappliance* Elecappliance;
	ECircuitType CircuitType;
	float CResistances;
	~FElecTree()
	{
		for (FElecTree*& Child:Childs)
		{delete Child;}
	}
	void CaculateResistance();
	void Electrify(float Voltage);
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
		int NexFlag = 0;
		TArray<int> PinCollection;
		void PushPin()
		{PinCollection.Push(CurIndex);}
		int PeekLastPin()
		{return PinCollection.Last();}
		void PopPin()
		{CurIndex = PinCollection.Pop();}
		//Return If Curindex is out of bound
		bool Next()
		{
			CurIndex++;
			return CurIndex >= ElecPath.Num();
		}

		//Return If Curindex is out of bound
		bool Prev()
		{
			CurIndex--;
			return CurIndex < 0;
		}
		AElecappliance* GetCurrent()
		{return ElecPath[CurIndex];}
		static void GenCommonFlag(TArray<FElecPath*>& ElecPaths);
	};
	void Internal_Electrify(FElecLinkInfo BeginSearch, TArray<FElecPath>& ElecPaths, FElecPath& SearchMap);
	void Internal_GenElecTreeSeries(TArray<FElecPath*> ElecPaths,int FirstEndIndex,FElecTree* TargetTree);
	void Internal_GenElecTreeParallel(TArray<FElecPath*> ElecPaths,FElecTree* TargetTree);
public:
	
	//Battery will use It own Voltage instead of the Voltage parameter
	virtual void Electrify_Implementation(float Voltage) override;
	UPROPERTY(EditAnywhere)
		float Voltage;

	virtual TMap<FString, FQtPropertyInfo> CollectSyncableProperty_Implementation() override;
	virtual void OnPropertyValueChanged_Implementation(const FString& PropertyName, const FString& ValueStr) override;
};
