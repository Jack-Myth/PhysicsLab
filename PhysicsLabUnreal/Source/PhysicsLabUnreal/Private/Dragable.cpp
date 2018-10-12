// Fill out your copyright notice in the Description page of Project Settings.

#include "Dragable.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADragable::ADragable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("DefaultRoot");
}

// Called when the game starts or when spawned
void ADragable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADragable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsDraging)
		return;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjType;
	FHitResult HitR;
	ObjType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursorForObjects(ObjType,true,HitR);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Tick");
	if (HitR.bBlockingHit)
	{
		SetActorLocation(HitR.Location);
	}
}

void ADragable::NotifyActorOnClicked(FKey ButtonPressed /*= EKeys::LeftMouseButton*/)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	if (ButtonPressed==EKeys::LeftMouseButton)
	{
		FHitResult HitR;
		UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursor(ECC_Visibility, true, HitR);
		if (HitR.GetActor()==this&&DragExcludeComponent().Find(HitR.GetComponent()) == INDEX_NONE)
		{
			bIsDraging = true;
		}
	}
}

void ADragable::NotifyActorOnReleased(FKey ButtonReleased /*= EKeys::LeftMouseButton*/)
{
	Super::NotifyActorOnReleased(ButtonReleased);
	if (ButtonReleased == EKeys::LeftMouseButton)
		bIsDraging = false;
}

TArray<class USceneComponent*> ADragable::DragExcludeComponent_Implementation()
{
	return TArray<USceneComponent*>();
}