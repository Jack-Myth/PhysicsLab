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
	PositiveP->SetupAttachment(RootComponent);
	NegativeP->SetupAttachment(RootComponent);
	PositiveP->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")).Object);
	UMaterial* PositiveMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/PhysicsLab/Materials/PoleMat.PoleMat'")).Object;
	PositiveP->SetMaterial(0, PositiveMat);
	PositiveP->SetWorldScale3D(FVector(0.01, 0.01, 0.01));
	NegativeP->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")).Object);
	NegativeP->SetWorldScale3D(FVector(0.01, 0.01, 0.01));
	UMaterial* NegativeMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/PhysicsLab/Materials/PoleMat.PoleMat'")).Object;
	NegativeP->SetMaterial(0, NegativeMat);
}

void AElecappliance::OnPoleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Cast<ADefGameModeBase>(UGameplayStatics::GetGameMode(this))->SubmitClickedElecPole((UStaticMeshComponent*)TouchedComponent);
}

void AElecappliance::LinkPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor)
{
	LinkMap.FindOrAdd(InSelfActor).AddUnique(InOtherActor);
}

void AElecappliance::BreakPole(class UStaticMeshComponent* InSelfActor, class UStaticMeshComponent* InOtherActor)
{
	LinkMap.FindOrAdd(InSelfActor).Remove(InOtherActor);
}

TArray<class USceneComponent*> AElecappliance::DragExcludeComponent_Implementation()
{
	TArray<class USceneComponent*> ExcludeCollection=Super::DragExcludeComponent_Implementation();
	ExcludeCollection.Add(PositiveP);
	ExcludeCollection.Add(NegativeP);
	return ExcludeCollection;
}

FName AElecappliance::FindPoleNameByComponent(class UStaticMeshComponent* PoleComponent)
{
	if (PositiveP == PoleComponent)
		return "PositiveP";
	else if (NegativeP == PoleComponent)
		return "NegativeP";
	else
		return "None";
}

void AElecappliance::BeginPlay()
{
	Super::BeginPlay();
	PositiveDMI = PositiveP->CreateDynamicMaterialInstance(0);
	PositiveDMI->SetVectorParameterValue("Color", FLinearColor(1, 0, 0));
	NegativeDMI = NegativeP->CreateDynamicMaterialInstance(0);
	NegativeDMI->SetVectorParameterValue("Color", FLinearColor(0, 0, 1));
	EnableInput(UGameplayStatics::GetPlayerController(this, 0));
	PositiveP->OnClicked.AddDynamic(this, &AElecappliance::OnPoleClicked);
	NegativeP->OnClicked.AddDynamic(this, &AElecappliance::OnPoleClicked);
}
