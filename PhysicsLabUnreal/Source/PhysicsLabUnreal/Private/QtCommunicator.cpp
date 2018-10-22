// Fill out your copyright notice in the Description page of Project Settings.

#include "QtCommunicator.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "CommandLine.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "JsonReader.h"
#include "JsonObject.h"
#include "JsonSerializer.h"
#include "Engine/GameViewportClient.h"
#include "SWindow.h"
#include "GenericWindow.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <stack>
#include "EngineUtils.h"
#include "Interactable.h"
#include "Particles/ParticleSystem.h"

WNDPROC AQtCommunicator::OriginalWndProc;

LRESULT AQtCommunicator::ModifiedWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	//Process WM_MOVE for child window
	if (msg == WM_MOVE)
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);
		short* plp = (short*)&lp;
		*plp = (short)rc.left; //low
		*(plp + 1) = (short)rc.top; //high
	}
	return OriginalWndProc(hwnd, msg, wp, lp);
}

TSharedPtr<FJsonValue> AQtCommunicator::Internal_CollectActorChild(AActor* TargetActor, TArray<AActor*>& SceneActorCollectionRef)
{
	TArray<AActor*> ChildCollection;
	TargetActor->GetAllChildActors(ChildCollection, false);
	TSharedPtr<FJsonValue> TargetJsonValue;
	if (ChildCollection.Num())
	{
		FJsonObject* Obj = new FJsonObject();
		TArray<TSharedPtr<FJsonValue>> ChildList;
		for (AActor*& CurActor:ChildCollection)
			ChildList.Push(Internal_CollectActorChild(CurActor, SceneActorCollectionRef));
		Obj->SetArrayField(TargetActor->GetName(), ChildList);

		TargetJsonValue = MakeShareable(new FJsonValueObject(MakeShareable(Obj)));
	}
	else
		TargetJsonValue = MakeShareable(new FJsonValueString(TargetActor->GetName()));
	SceneActorCollectionRef.Remove(TargetActor);
	return TargetJsonValue;
}

void AQtCommunicator::TryConnect()
{
	FTcpSocketBuilder tcpBuilder("Make Communication with QtClient");
	QtCommunicator= tcpBuilder.AsNonBlocking().AsReusable().Build();
	TSharedRef<FInternetAddr> LocalQtAddr= ISocketSubsystem::Get()->CreateInternetAddr(FIPv4Address(127,0,0,1).Value, 10703);
	if (QtCommunicator->Connect(LocalQtAddr.Get()))
	{
		GetWorld()->GetTimerManager().SetTimer(CommunicatorTimerHandler, this, &AQtCommunicator::CheckPendingMsgData, 0.016f, true);
	}
}

void AQtCommunicator::SyncSceneToQt()
{
	TArray<AActor*> ActorCollection;
	FJsonObject ActorListJson;
	UGameplayStatics::GetAllActorsWithInterface(this,UInteractable::StaticClass(),ActorCollection);
	ActorCollection.Remove(this); //Remove Communicator itself;
	TArray<TSharedPtr<FJsonValue>> SceneActorList;
	while (ActorCollection.Num())
	{
		if (!ActorCollection[0]->GetParentActor())
			SceneActorList.Add(Internal_CollectActorChild(ActorCollection[0], ActorCollection));
		else
			ActorCollection.RemoveAt(0);
	}
	ActorListJson.SetStringField("Action", "SyncScene");
	ActorListJson.SetArrayField("ActorList", SceneActorList);
	SendJson(ActorListJson);
}

/*#define GEN_PROPERTY(PROPERTY_NAME,TYPE) \
TSharedPtr<FJsonObject> PROPERTY_NAME = MakeShareable(new FJsonObject());\
PROPERTY_NAME->SetStringField("Type", TYPE);*/
void AQtCommunicator::SyncActorDetails(AActor* TargetActor)
{
	if (!TargetActor->Implements<UInteractable>())
		return;
	FJsonObject ActorDetailJson;
	TSharedPtr<FJsonObject> PropertyListJson=MakeShareable(new FJsonObject());
	//Property List
	{
		TMap<FString, FQtPropertyInfo> PropertyMap = IInteractable::Execute_CollectSyncableProperty(TargetActor);
		for (auto it = PropertyMap.CreateConstIterator();it;++it)
		{
			TSharedPtr<FJsonObject> PropertyJson = MakeShareable(new FJsonObject());
			PropertyJson->SetStringField("DisplayName", it->Value.DisplayName);
			switch (it->Value.Type)
			{
				case EQtPropertyType::QPT_Float:
					PropertyJson->SetStringField("Type", "Float");
					PropertyJson->SetNumberField("Value", FCString::Atof(*(it->Value.ValueStr)));
					break;
				case EQtPropertyType::QPT_String:
					PropertyJson->SetStringField("Type", "String");
					PropertyJson->SetStringField("Value",it->Value.ValueStr);
					break;
				case EQtPropertyType::QPT_UserDefined:
					PropertyJson->SetStringField("Type", "UserDefined");
					PropertyJson->SetStringField("Value", it->Value.ValueStr);
					break;
			}
			PropertyListJson->SetObjectField(it->Key, PropertyJson);
		}
	}
	ActorDetailJson.SetStringField("Action", "SyncActorDetails");
	ActorDetailJson.SetObjectField("Properties", PropertyListJson);
	SendJson(ActorDetailJson);
}

void AQtCommunicator::SendMsg(const TArray<char>& Data)
{
	int DataSize=Data.Num();
	TArray<char> TargetData;
	TargetData.SetNum(DataSize + sizeof(int),false);
	FMemory::Memcpy(TargetData.GetData(), &DataSize, sizeof(int));
	FMemory::Memcpy(TargetData.GetData() + sizeof(int), Data.GetData(), DataSize);
	int byteSent;
	QtCommunicator->Send((uint8*)TargetData.GetData(), TargetData.Num(), byteSent);
}

void AQtCommunicator::SendJson(const FJsonObject& JsonToSend)
{
	FString JsonStr;
	auto JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonStr, 0);
	FJsonSerializer::Serialize(MakeShared<FJsonObject>(JsonToSend), JsonWriter);
	TArray<char> DataCollection;
	auto twoHundredAnsi = StringCast<ANSICHAR>(*JsonStr);
	DataCollection.Append(twoHundredAnsi.Get(), twoHundredAnsi.Length());
	DataCollection.Push(0);
	SendMsg(DataCollection);
}

void AQtCommunicator::RequestHwnd()
{
	void* Hwnd = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	OriginalWndProc = (WNDPROC)SetWindowLongPtr((HWND)Hwnd, GWLP_WNDPROC, (LONG_PTR)&AQtCommunicator::ModifiedWndProc);
	FJsonObject newJson;
	newJson.SetStringField("Action", "SendHwnd");
	newJson.SetStringField("Hwnd", FString::Printf(TEXT("%lld"), (long long)Hwnd));
	SendJson(newJson);
}

void AQtCommunicator::SelectActor()
{
	if (IsValid(SelectedActor))
		IInteractable::Execute_OnActorDeselected(SelectedActor,SelectedActor,this);
	FString ActorName = TargetMsg->GetStringField("ActorName");
	TArray<AActor*> InteractableActors;
	UGameplayStatics::GetAllActorsWithInterface(this, UInteractable::StaticClass(), InteractableActors);
	for (AActor*& InteractableActor:InteractableActors)
	{
		if (InteractableActor->GetName() == ActorName)
		{
			SelectedActor = InteractableActor;
			break;
		}
	}
	IInteractable::Execute_OnActorSelected(SelectedActor, SelectedActor,this);
}

void AQtCommunicator::Quit()
{
	//Reset WndProc
	void* Hwnd = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	SetWindowLongPtr((HWND)Hwnd, GWLP_WNDPROC, (LONG_PTR)OriginalWndProc);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit);
}

void AQtCommunicator::RequestRefresh()
{
	SyncSceneToQt();
}

void AQtCommunicator::SendActorDetail()
{
	FString PropertyName = TargetMsg->GetStringField("Name");
	FString PropertyType = TargetMsg->GetStringField("Type");
	if (PropertyType=="Float")
	{
		double Value = TargetMsg->GetNumberField("Value");
		if (SelectedActor)
			IInteractable::Execute_OnPropertyValueChanged(SelectedActor, PropertyName, FString::SanitizeFloat(Value));
	}
	else
	{
		FString Value = TargetMsg->GetStringField("Value");
		if (SelectedActor)
			IInteractable::Execute_OnPropertyValueChanged(SelectedActor, PropertyName, Value);
	}
}

void AQtCommunicator::SpawnBPClass()
{
	FString UClassPath = TargetMsg->GetStringField("UClassPath");
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, UClassPath);
	UClass* TargetClass = LoadClass<AActor>(nullptr, *UClassPath);
	if (TargetClass)
	{
		TArray<AActor*> SpawnPointCollection;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SpawnPoint", SpawnPointCollection);
		if (SpawnPointCollection.Num())
		{
			AActor* TargetActor = GetWorld()->SpawnActor<AActor>(TargetClass,SpawnPointCollection[0]->GetActorLocation(),FRotator::ZeroRotator);
			UParticleSystem* SpawnEffect= LoadObject<UParticleSystem>(nullptr, TEXT("ParticleSystem'/Game/PhysicsLab/Particles/SpawnEffect.SpawnEffect'"));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnEffect, SpawnPointCollection[0]->GetActorLocation() + FVector(0, 0, 5), FRotator::ZeroRotator, false);
		}
	}
}

void AQtCommunicator::InvokeAction()
{
	auto JsonReader = TJsonReaderFactory<TCHAR>::Create(FString(CachedMessage.CacheMessageData.GetData()));
	TSharedPtr<FJsonObject> JsonParsed = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(JsonReader, JsonParsed);
	TargetMsg = JsonParsed.Get();
	FString ActionName = JsonParsed->GetStringField("Action");
	UFunction* ActionTarget = this->FindFunction(*ActionName);
	if (ActionTarget)
	{
		this->ProcessEvent(ActionTarget, nullptr);
	}
	TargetMsg = nullptr;
}

void AQtCommunicator::CheckPendingMsgData()
{
	uint32 PendingDataSize;
	if (QtCommunicator->HasPendingData(PendingDataSize))
	{
		if (CachedMessage.TargetLength)
		{
			int bufferRead=0;
			QtCommunicator->Recv((uint8*)CachedMessage.CacheMessageData.GetData() + CachedMessage.CurrentLength,
				CachedMessage.TargetLength - CachedMessage.CurrentLength, bufferRead);
			CachedMessage.CurrentLength += bufferRead;
		}
		else if (PendingDataSize > sizeof(int))
		{
			QtCommunicator->Recv((uint8*)&CachedMessage.TargetLength, sizeof(int), CachedMessage.CurrentLength);
			CachedMessage.CacheMessageData.SetNum(CachedMessage.TargetLength, false);
			QtCommunicator->Recv((uint8*)CachedMessage.CacheMessageData.GetData(), CachedMessage.CacheMessageData.Num(), CachedMessage.CurrentLength);
		}
		if (CachedMessage.CurrentLength==CachedMessage.TargetLength&&CachedMessage.TargetLength>0)
		{
			InvokeAction();
			CachedMessage.CacheMessageData.Empty();
			CachedMessage.CurrentLength = 0;
			CachedMessage.TargetLength = 0;
		}
	}
}


