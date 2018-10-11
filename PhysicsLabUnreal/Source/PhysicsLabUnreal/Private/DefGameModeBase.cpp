// Fill out your copyright notice in the Description page of Project Settings.

#include "DefGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "QtCommunicator.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Elecappliance.h"




class AQtCommunicator* ADefGameModeBase::GetQtCommunicator()
{
	return QtCommunicator;
}

class AQtCommunicator* ADefGameModeBase::GetQtCommunicator(AActor* ContextActor)
{
	ADefGameModeBase* DefGameModePtr = Cast<ADefGameModeBase>(ContextActor);
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
	if (this->ElecPole==ElecPole)
	{
		this->ElecPole = nullptr;
		((UMaterialInstanceDynamic*)ElecPole->GetMaterial(0))->SetScalarParameterValue("Emissive", 10.f);
	}
	else
	{
		AElecappliance* ElecapplianceA,*ElecapplianceB;
		ElecapplianceA = (AElecappliance*)this->ElecPole->GetOwner();
		ElecapplianceA->LinkPole(this->ElecPole, ElecPole);
		ElecapplianceB = (AElecappliance*)ElecPole->GetOwner();
		ElecapplianceB->LinkPole(ElecPole, this->ElecPole);
		//TODO: Gen Cable Actor
	}
}
