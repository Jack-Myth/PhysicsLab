// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LabStudent.generated.h"

UCLASS()
class PHYSICSLABUNREAL_API ALabStudent : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* Mesh=nullptr;
public:
	// Sets default values for this pawn's properties
	ALabStudent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsMovable=false;
	UFUNCTION()
		void MoveForward(float Axis);
	UFUNCTION()
		void MoveRight(float Axis);
	UFUNCTION()
		void ViewUp(float Axis);
	UFUNCTION()
		void ViewTurn(float Axis);
	UFUNCTION()
		void RightMouseButtonPressed();
	UFUNCTION()
		void RightMouseButtonReleased();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
