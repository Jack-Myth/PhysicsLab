// Fill out your copyright notice in the Description page of Project Settings.

#include "ModifiedDefaultPawn.h"


void AModifiedDefaultPawn::EnableMovement()
{
	AllowMovement = true;
}

void AModifiedDefaultPawn::DisableMovement()
{
	AllowMovement = false;
}

void AModifiedDefaultPawn::MoveForward(float Val)
{
	if (AllowMovement)
		Super::MoveForward(Val);
}

void AModifiedDefaultPawn::MoveRight(float Val)
{
	if (AllowMovement)
		Super::MoveRight(Val);
}

void AModifiedDefaultPawn::MoveUp_World(float Val)
{
	if (AllowMovement)
		Super::MoveUp_World(Val);
}

void AModifiedDefaultPawn::TurnAtRate(float Rate)
{
	if (AllowMovement)
		Super::TurnAtRate(Rate);
}

void AModifiedDefaultPawn::LookUpAtRate(float Rate)
{
	if (AllowMovement)
		Super::LookUpAtRate(Rate);
}

void AModifiedDefaultPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);
	InInputComponent->BindAction("RightMouse", EInputEvent::IE_Pressed, this, &AModifiedDefaultPawn::EnableMovement);
	InInputComponent->BindAction("RightMouse", EInputEvent::IE_Released, this, &AModifiedDefaultPawn::DisableMovement);
}
