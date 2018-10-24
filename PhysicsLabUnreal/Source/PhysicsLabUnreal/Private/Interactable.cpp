// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "QtCommunicator.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"


// Add default functionality here for any IInteractable functions that are not pure virtual.

void IInteractable::OnActorSelected_Implementation(UObject* ObjBasePtr, AQtCommunicator* QtCommunicatorC)
{
	if (!IsValid(QtCommunicatorC))
		return;
	QtCommunicatorC->SyncActorDetails(Cast<AActor>(ObjBasePtr));
	UProperty* pp = ObjBasePtr->GetClass()->FindPropertyByName("bIsSelected");
	if (pp)
		*(pp->ContainerPtrToValuePtr<bool>(ObjBasePtr)) = true;
	TArray<UActorComponent*> StaticMeshComponents = Cast<AActor>(ObjBasePtr)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (UActorComponent*& StaticMeshComponent:StaticMeshComponents)
	{
		((UStaticMeshComponent*)StaticMeshComponent)->SetRenderCustomDepth(true);
	}
}

void IInteractable::OnActorDeselected_Implementation(UObject* ObjBasePtr, AQtCommunicator* QtCommunicatorC)
{
	TArray<UActorComponent*> StaticMeshComponents = Cast<AActor>(ObjBasePtr)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	QtCommunicatorC->ClearDataPoint();
	UProperty* pp = ObjBasePtr->GetClass()->FindPropertyByName("bIsSelected");
	if (pp)
		*(pp->ContainerPtrToValuePtr<bool>(ObjBasePtr)) = false;
	for (UActorComponent*& StaticMeshComponent : StaticMeshComponents)
	{
		((UStaticMeshComponent*)StaticMeshComponent)->SetRenderCustomDepth(false);
	}
}
