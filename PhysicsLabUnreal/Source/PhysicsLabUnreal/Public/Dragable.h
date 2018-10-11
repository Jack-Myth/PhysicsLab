// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dragable.generated.h"

UCLASS()
class PHYSICSLABUNREAL_API ADragable : public AActor
{
	GENERATED_BODY()

	bool bIsDraging;
public:	
	// Sets default values for this actor's properties
	ADragable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	virtual void NotifyActorOnReleased(FKey ButtonReleased = EKeys::LeftMouseButton) override;
	UFUNCTION(BlueprintNativeEvent)
		TArray<class USceneComponent*> DragExcludeComponent();

};
