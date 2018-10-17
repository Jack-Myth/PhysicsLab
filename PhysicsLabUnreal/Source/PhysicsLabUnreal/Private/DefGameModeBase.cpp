// Fill out your copyright notice in the Description page of Project Settings.

#include "DefGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "QtCommunicator.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Elecappliance.h"
#include "Classes/CableActor.h"
#include "CableComponent.h"
#include "Engine/World.h"


class AQtCommunicator* ADefGameModeBase::GetQtCommunicator()
{
	return QtCommunicator;
}

class AQtCommunicator* ADefGameModeBase::GetQtCommunicator(AActor* ContextActor)
{
	ADefGameModeBase* DefGameModePtr = Cast	<ADefGameModeBase>(ContextActor);
	if (IsValid(DefGameModePtr))
		return DefGameModePtr->GetQtCommunicator();
	else
	{
		if (IsValid(ContextActor))
		{
			TArray<AActor*> ResCollection;
			UGameplayStatics::GetAllActorsOfClass(ContextActor, AQtCommunicator::StaticClass(), ResCollection);
			if (ResCollection.Num())
				return (AQtCommunicator*)ResCollection[0];
		}
		return nullptr;
	}
}

void ADefGameModeBase::SubmitClickedElecPole(UStaticMeshComponent* ElecPole)
{
	if (!this->ElecPole)
	{
		//Use As the first Click.
		this->ElecPole = ElecPole;
		((UMaterialInstanceDynamic*)ElecPole->GetMaterial(0))->SetScalarParameterValue("Emissive", 10.f);
		return;
	}
	if (this->ElecPole==ElecPole)
	{
		//Check if click the same pole
		this->ElecPole = nullptr;
		((UMaterialInstanceDynamic*)ElecPole->GetMaterial(0))->SetScalarParameterValue("Emissive", 0.f);
	}
	else
	{
		if (UGameplayStatics::GetPlayerController(this, 0)->IsInputKeyDown(EKeys::LeftAlt))
		{
			for (auto it=CableLinkedMap.CreateIterator();it;++it)
			{
				//Find two pole
				if (it.Value().Find(this->ElecPole) != INDEX_NONE && it.Value().Find(ElecPole))
				{
					AElecappliance* ElecapplianceA, *ElecapplianceB;
					ElecapplianceA = (AElecappliance*)this->ElecPole->GetOwner();
					ElecapplianceA->BreakPole(this->ElecPole, ElecPole);
					ElecapplianceB = (AElecappliance*)ElecPole->GetOwner();
					ElecapplianceB->BreakPole(ElecPole, this->ElecPole);
					it.Key()->Destroy();
					break;
				}
				((UMaterialInstanceDynamic*)this->ElecPole->GetMaterial(0))->SetScalarParameterValue("Emissive", 0.f);
			}
			this->ElecPole = nullptr;
		}
		else
		{
			AElecappliance* ElecapplianceA, *ElecapplianceB;
			ElecapplianceA = (AElecappliance*)this->ElecPole->GetOwner();
			if (ElecapplianceA->GetPoleConenction(this->ElecPole).Find(ElecPole) != INDEX_NONE)
			{
				this->ElecPole = nullptr;
				((UMaterialInstanceDynamic*)this->ElecPole->GetMaterial(0))->SetScalarParameterValue("Emissive", 0.f);
				return;
			}
			ElecapplianceA->LinkPole(this->ElecPole, ElecPole);
			ElecapplianceB = (AElecappliance*)ElecPole->GetOwner();
			ElecapplianceB->LinkPole(ElecPole, this->ElecPole);
			AActor* CableA = GetWorld()->SpawnActor<AActor>(LoadClass<AActor>(nullptr,TEXT("Blueprint'/Game/PhysicsLab/Blueprints/PhysicsLab/Wire.Wire_C'")),this->ElecPole->GetComponentLocation(),FRotator::ZeroRotator);
			UCableComponent* CableComponent = (UCableComponent*)CableA->GetRootComponent();
			CableA->AttachToComponent(this->ElecPole, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			CableComponent->SetAttachEndTo(ElecPole->GetOwner(), ElecapplianceB->FindPoleNameByComponent(ElecPole));
			TArray<UStaticMeshComponent*> LinkedMapElement;
			LinkedMapElement.Add(this->ElecPole);
			LinkedMapElement.Add(ElecPole);
			CableLinkedMap.FindOrAdd(CableA) = LinkedMapElement;
			((UMaterialInstanceDynamic*)this->ElecPole->GetMaterial(0))->SetScalarParameterValue("Emissive", 0.f);
			this->ElecPole = nullptr;
		}
	}
}
