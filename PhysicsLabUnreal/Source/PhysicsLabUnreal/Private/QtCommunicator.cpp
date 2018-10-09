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
	//Collect Actor Transform
	{
		TSharedPtr<FJsonObject> TransformProperty = MakeShareable(new FJsonObject());
		FTransform TransformValue = TargetActor->GetActorTransform();
		TSharedPtr<FJsonObject> LocationJson = MakeShareable(new FJsonObject());
		TSharedPtr<FJsonObject> RotationJson = MakeShareable(new FJsonObject());
		TSharedPtr<FJsonObject> ScaleJson = MakeShareable(new FJsonObject());
		auto VectorLambda = [](TSharedPtr<FJsonObject>& JsonObj, float X, float Y, float Z)
		{
			JsonObj->SetStringField("X", FString::SanitizeFloat(X));
			JsonObj->SetStringField("Y", FString::SanitizeFloat(Y));
			JsonObj->SetStringField("Z", FString::SanitizeFloat(Z));
		};
		VectorLambda(LocationJson, TransformValue.GetLocation().X,
			TransformValue.GetLocation().Y, TransformValue.GetLocation().Z);
		VectorLambda(RotationJson, TransformValue.GetRotation().X,
			TransformValue.GetRotation().Y, TransformValue.GetRotation().Z);
		VectorLambda(ScaleJson, TransformValue.GetScale3D().X,
			TransformValue.GetScale3D().Y, TransformValue.GetScale3D().Z);
		TransformProperty->SetObjectField("Location", LocationJson);
		TransformProperty->SetObjectField("Rotation", RotationJson);
		TransformProperty->SetObjectField("Scale", ScaleJson);
		PropertyListJson->SetObjectField("__ActorTransform", TransformProperty);
	}
	//Collect Other Property
	{
		TMap<FString, FQtPropertyInfo> PropertyMap = IInteractable::Execute_CollectSyncableProperty(TargetActor);
		for (auto it = PropertyMap.CreateConstIterator();it;++it)
		{
			TSharedPtr<FJsonObject> PropertyJson = MakeShareable(new FJsonObject());
			PropertyJson->SetStringField("DisplayName", it->Value.DisplayName);
			PropertyJson->SetStringField("Type", it->Value.Type);
			PropertyJson->SetStringField("Value", it->Value.ValueStr);
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
	FString ActorName = TargetMsg->GetStringField("ActorName");
	for (TActorIterator<AActor> ActorIt(GetWorld());ActorIt;++ActorIt)
	{
		if ((*ActorIt)->GetName() == ActorName)
		{
			SelectedActor = *ActorIt;
			break;
		}
	}
	SyncActorDetails(SelectedActor);
}

void AQtCommunicator::Quit()
{
	//Reset WndProc
	void* Hwnd = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	SetWindowLongPtr((HWND)Hwnd, GWLP_WNDPROC, (LONG_PTR)OriginalWndProc);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit);
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


