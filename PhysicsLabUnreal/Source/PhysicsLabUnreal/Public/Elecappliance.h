// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dragable.h"
#include "Interactable.h"
#include "Engine/StaticMeshActor.h"
#include "Elecappliance.generated.h"

/**
 * 
 */
UCLASS()
class PHYSICSLABUNREAL_API AElecappliance : public ADragable, public IInteractable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, AdvancedDisplay)
		class UStaticMeshComponent* PositiveP;
	UPROPERTY(EditAnywhere, AdvancedDisplay)
		class UStaticMeshComponent* NegativeP;
	class UMaterialInstanceDynamic* PositiveDMI;
	class UMaterialInstanceDynamic* NegativeDMI;
	TMap<class UStaticMeshComponent*, TArray<class UStaticMeshComponent*>> LinkMap;
public:
	AElecappliance();
	
	UFUNCTION()
		void OnPositivePoleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	void LinkPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor);
	void BreakPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor);
protected:
	virtual void BeginPlay() override;
};
