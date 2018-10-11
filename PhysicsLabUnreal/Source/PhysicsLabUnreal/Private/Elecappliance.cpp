// Fill out your copyright notice in the Description page of Project Settings.

#include "Elecappliance.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "DefGameModeBase.h"

AElecappliance::AElecappliance()
{
	PositiveP = CreateDefaultSubobject<UStaticMeshComponent>("PositivePole");
	NegativeP = CreateDefaultSubobject<UStaticMeshComponent>("NegativePole");
	PositiveP->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")).Object);
	UMaterial* PositiveMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/PhysicsLab/Materials/PoleMat.PoleMat'")).Object;
	PositiveP->SetMaterial(0, PositiveMat);
	PositiveP->SetWorldScale3D(FVector(0.01, 0.01, 0.01));
	NegativeP->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")).Object);
	NegativeP->SetWorldScale3D(FVector(0.01, 0.01, 0.01));
	UMaterial* NegativeMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/PhysicsLab/Materials/PoleMat.PoleMat'")).Object;
	NegativeP->SetMaterial(0, NegativeMat);
	//PositiveP->OnClicked.AddDynamic(this, OnPositivePoleClicked());
}

void AElecappliance::OnPositivePoleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	ADefGameModeBase::GetQtCommunicator(UGameplayStatics::GetGameMode(this));
}

void AElecappliance::LinkPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor)
{
	LinkMap.FindOrAdd(InSelfActor).AddUnique(InOtherActor);
}

void AElecappliance::BreakPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor)
{
	LinkMap.FindOrAdd(InSelfActor).Remove(InOtherActor);
}

void AElecappliance::BeginPlay()
{
	PositiveDMI = PositiveP->CreateDynamicMaterialInstance(0);
	PositiveDMI->SetVectorParameterValue("Color", FLinearColor(1, 0, 0));
	NegativeDMI = NegativeP->CreateDynamicMaterialInstance(0);
	NegativeDMI->SetVectorParameterValue("Color", FLinearColor(0, 0, 1));
}
