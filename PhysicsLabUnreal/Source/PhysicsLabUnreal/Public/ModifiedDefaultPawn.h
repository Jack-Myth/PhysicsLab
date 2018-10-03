// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "ModifiedDefaultPawn.generated.h"

/**
 * 
 */
UCLASS()
class PHYSICSLABUNREAL_API AModifiedDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()

	bool AllowMovement=false;

	void EnableMovement();

	void DisableMovement();

	virtual void MoveForward(float Val) override;


	virtual void MoveRight(float Val) override;


	virtual void MoveUp_World(float Val) override;


	virtual void TurnAtRate(float Rate) override;


	virtual void LookUpAtRate(float Rate) override;


	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

};
