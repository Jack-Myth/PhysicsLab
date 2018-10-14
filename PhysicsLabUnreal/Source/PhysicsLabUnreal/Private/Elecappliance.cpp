// Fill out your copyright notice in the Description page of Project Settings.

#include "Elecappliance.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "DefGameModeBase.h"
#include "Battery.h"

ABattery* AElecappliance::FindBattery()
{
	TMap<AElecappliance*, bool> IsSearched;
	FElecLinkInfo StartSearchInfo;
	StartSearchInfo.Elecappliance = this;
	StartSearchInfo.ExitPole = PositiveP;
	TArray<AElecappliance*> SearchMap;
	ABattery* tmpBattery = Internal_FindBatery(StartSearchInfo,SearchMap);
	if (tmpBattery)
		return tmpBattery;
	StartSearchInfo.ExitPole = NegativeP;
	SearchMap.Empty();
	return Internal_FindBatery(StartSearchInfo,SearchMap);
}

TArray<FElecLinkInfo> AElecappliance::Internal_GetNextLinks(class UStaticMeshComponent* TemplatePole)
{
	TArray<FElecLinkInfo> NextLinks;
	AElecappliance* ElecAppliance = Cast<AElecappliance>(TemplatePole->GetOwner());
	FElecLinkInfo thisLinkInfo;
	thisLinkInfo.Elecappliance = Cast<AElecappliance>(TemplatePole->GetOwner());
	thisLinkInfo.ExitPole = TemplatePole;
	NextLinks.Add(thisLinkInfo);
	const TArray<UStaticMeshComponent*> Connections = thisLinkInfo.Elecappliance->GetPoleConenction(TemplatePole);
	for (UStaticMeshComponent*const& NextPole:Connections)
	{
		if (NextPole->GetOwner()!=thisLinkInfo.Elecappliance)
			NextLinks.Append(Internal_GetNextLinks(NextPole));
	}
	return NextLinks;
}

ABattery* AElecappliance::Internal_FindBatery(const FElecLinkInfo& SearchBegin, TArray<AElecappliance*>& SearchMap)
{
	if (SearchBegin.Elecappliance&&SearchMap.Find(SearchBegin.Elecappliance)==INDEX_NONE)
	{
		SearchMap.Add(SearchBegin.Elecappliance);
		const TArray<UStaticMeshComponent*>& SearchNext = SearchBegin.Elecappliance->GetPoleConenction(SearchBegin.ExitPole);
		for (UStaticMeshComponent* const& NextPoleComponent : SearchNext)
		{
			if (NextPoleComponent->GetOwner()->GetClass() == ABattery::StaticClass())
				return Cast<ABattery>(NextPoleComponent->GetOwner());
			else
			{
				FElecLinkInfo Nextlink;
				Nextlink.Elecappliance = Cast<AElecappliance>(NextPoleComponent->GetOwner());
				Nextlink.ExitPole = NextPoleComponent;
				Nextlink.ExitPole = Nextlink.Elecappliance->PositiveP;
				ABattery* Searchresult = Internal_FindBatery(Nextlink, SearchMap);
				if (Searchresult)
					return Searchresult;
				Nextlink.ExitPole = Nextlink.Elecappliance->NegativeP;
				Searchresult = Internal_FindBatery(Nextlink,SearchMap);
				if (Searchresult)
					return Searchresult;
			}
		}
	}
	return nullptr;
}

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

void AElecappliance::UpdateElecState()
{
	
}

TArray<FElecLinkInfo> AElecappliance::GetNextLinksStatic(class UStaticMeshComponent* TemplatePole)
{
	AElecappliance* ElecapplianceOwner = Cast<AElecappliance>(TemplatePole->GetOwner());
	if (!ElecapplianceOwner)
		return TArray<FElecLinkInfo>();
	return ElecapplianceOwner->GetNextLinks(TemplatePole);
}

TArray<FElecLinkInfo> AElecappliance::GetNextLinks(class UStaticMeshComponent* TemplatePole)
{
	TArray<FElecLinkInfo> NextLinks;
	const TArray<UStaticMeshComponent*> Connections = GetPoleConenction(TemplatePole);
	for (UStaticMeshComponent* const& NextPole : Connections)
	{
		if (NextPole->GetOwner() != this)
			NextLinks.Append(Internal_GetNextLinks(NextPole));
	}
	return NextLinks;
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
