// Fill out your copyright notice in the Description page of Project Settings.

#include "LabStudent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/MeshComponent.h"


// Sets default values
ALabStudent::ALabStudent()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ALabStudent::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALabStudent::MoveForward(float Axis)
{
	if (bIsMovable)
		AddMovementInput(UKismetMathLibrary::GetForwardVector(GetControlRotation()), Axis);
}

void ALabStudent::MoveRight(float Axis)
{

	if (bIsMovable)
		AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation()), Axis);
}

void ALabStudent::ViewUp(float Axis)
{
	if (bIsMovable)
		AddControllerPitchInput(-Axis);
}

void ALabStudent::ViewTurn(float Axis)
{
	if (bIsMovable)
		AddControllerYawInput(Axis);
}

void ALabStudent::RightMouseButtonPressed()
{
	bIsMovable = true;
}

void ALabStudent::RightMouseButtonReleased()
{
	bIsMovable = false;
}

// Called every frame
void ALabStudent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALabStudent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ALabStudent::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALabStudent::MoveRight);
	PlayerInputComponent->BindAxis("ViewUp", this, &ALabStudent::ViewUp);
	PlayerInputComponent->BindAxis("ViewTurn", this, &ALabStudent::ViewTurn);
	PlayerInputComponent->BindAction("RightMouse", EInputEvent::IE_Pressed, this, &ALabStudent::RightMouseButtonPressed);
	PlayerInputComponent->BindAction("RightMouse", EInputEvent::IE_Released, this, &ALabStudent::RightMouseButtonReleased);
}

