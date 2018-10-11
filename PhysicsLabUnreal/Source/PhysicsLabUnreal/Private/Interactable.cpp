// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "QtCommunicator.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"


// Add default functionality here for any IInteractable functions that are not pure virtual.

void IInteractable::OnActorSelected_Implementation()
{
	TArray<AActor*> QtCommunicatorC;
	UGameplayStatics::GetAllActorsOfClass((AActor*)this, AQtCommunicator::StaticClass(),QtCommunicatorC);
	if (!QtCommunicatorC.Num())
		return;
	((AQtCommunicator*)QtCommunicatorC[0])->SyncActorDetails((AActor*)this);
	TArray<UActorComponent*> StaticMeshComponents = ((AActor*)this)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (UActorComponent*& StaticMeshComponent:StaticMeshComponents)
	{
		((UStaticMeshComponent*)StaticMeshComponent)->bRenderCustomDepth = true;
	}
}

void IInteractable::OnActorDeselected_Implementation()
{
	TArray<UActorComponent*> StaticMeshComponents = ((AActor*)this)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (UActorComponent*& StaticMeshComponent : StaticMeshComponents)
	{
		((UStaticMeshComponent*)StaticMeshComponent)->bRenderCustomDepth = false;
	}
}
