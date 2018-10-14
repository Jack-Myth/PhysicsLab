// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dragable.h"
#include "Interactable.h"
#include "Engine/StaticMeshActor.h"
#include "Elecappliance.generated.h"

USTRUCT(BlueprintType)
struct FElecLinkInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		class AElecappliance* Elecappliance;
	UPROPERTY(BlueprintReadWrite)
		class UStaticMeshComponent* ExitPole;
};

/**
 * 
 */
UCLASS()
class PHYSICSLABUNREAL_API AElecappliance : public ADragable, public IInteractable
{
	GENERATED_BODY()

	friend class ABattery;
	class UMaterialInstanceDynamic* PositiveDMI;
	class UMaterialInstanceDynamic* NegativeDMI;
	TMap<class UStaticMeshComponent*, TArray<class UStaticMeshComponent*>> LinkMap;
	TArray<FElecLinkInfo> Internal_GetNextLinks(class UStaticMeshComponent* TemplatePole);
	ABattery* Internal_FindBatery(const FElecLinkInfo& SearchBegin,TArray<AElecappliance*>& SearchMap);
public:
	AElecappliance();
	UPROPERTY(EditAnywhere, AdvancedDisplay)
		class UStaticMeshComponent* PositiveP;
	UPROPERTY(EditAnywhere, AdvancedDisplay)
		class UStaticMeshComponent* NegativeP;
	UFUNCTION()
		void OnPoleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	void LinkPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor);
	void BreakPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor);
	TArray<class USceneComponent*> DragExcludeComponent_Implementation() override;
	FName FindPoleNameByComponent(class UStaticMeshComponent* PoleComponent);
	UFUNCTION(BlueprintCallable)
		void UpdateElecState();
	UFUNCTION(BlueprintPure)
		static TArray<FElecLinkInfo> GetNextLinksStatic(class UStaticMeshComponent* TemplatePole);
	UFUNCTION(BlueprintPure)
		TArray<FElecLinkInfo> GetNextLinks(class UStaticMeshComponent* TemplatePole);
	class ABattery* FindBattery();
	bool IsSearched = false;
	const TArray<class UStaticMeshComponent*> GetPoleConenction(class UStaticMeshComponent* Pole)
	{
		auto ptr = LinkMap.Find(Pole);
		return ptr ? *ptr : TArray<class UStaticMeshComponent*>();
	}
	UFUNCTION(BlueprintNativeEvent)
		void Electrify(float Voltage);
	virtual void Electrify_Implementation(float Voltage) {};
protected:
	virtual void BeginPlay() override;
};
