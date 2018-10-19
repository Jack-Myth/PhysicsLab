// Fill out your copyright notice in the Description page of Project Settings.

#include "Battery.h"

void ABattery::Internal_Electrify(FElecLinkInfo BeginSearch, TArray<FElecPath>& ElecPaths, FElecPath& SearchMap)
{
	if (SearchMap.ElecPath.Find(BeginSearch.Elecappliance) != INDEX_NONE)
		return;
	SearchMap.ElecPath.Push(BeginSearch.Elecappliance);
	BeginSearch.Elecappliance->Electrify(0);
	TArray<FElecLinkInfo> nextLinks = BeginSearch.Elecappliance->GetNextLinks(BeginSearch.ExitPole);
	for (FElecLinkInfo& NextLink : nextLinks)
	{
		if (NextLink.Elecappliance==this)
		{
			if (NextLink.ExitPole != NegativeP)
			{
				SearchMap.ElecPath.Push(this);
				ElecPaths.Add(SearchMap);
				SearchMap.ElecPath.Pop();
			}
			continue;
		}
		Internal_Electrify(NextLink,ElecPaths, SearchMap);
	}
	SearchMap.ElecPath.Pop();
}

void ABattery::Internal_GenElecTreeSeries(TArray<FElecPath> ElecPaths, int FirstEndIndex, FElecTree* TargetTree)
{
	TargetTree->CircuitType = ECircuitType::Series;
	while (true)
	{
		TArray<AElecappliance*> CurList;
		if (ElecPaths[0].CurIndex>=FirstEndIndex)
			break;
		for (int i=0;i<ElecPaths.Num();i++)
		{
			CurList.AddUnique(ElecPaths[i].GetCurrent());
		}
		if (CurList.Num()>1)
		{
			FElecTree* TargetT = new FElecTree();
			Internal_GenElecTreeParallel(ElecPaths, TargetT);
			TargetTree = TargetT;
			TargetTree->Childs.Add(TargetT);
		}
		else
		{
			if (CurList[0]->GetClass()->IsChildOf(ABattery::StaticClass())&&ElecPaths[0].CurIndex!=0)
				break;
			FElecTree* TargetT = new FElecTree();
			TargetT->Elecappliance = CurList[0];
			TargetTree->Childs.Add(TargetT);
			for (int i = 0; i < ElecPaths.Num(); i++)
				ElecPaths[i].Next();
		}
	}
}

void ABattery::Internal_GenElecTreeParallel(TArray<FElecPath> ElecPaths, FElecTree* TargetTree)
{
	TMap<AElecappliance*, TArray<FElecPath>> ElecPathMap;
	TargetTree->CircuitType = ECircuitType::Parallel;
	for (int i=0;i<ElecPaths.Num();i++)
	{
		ElecPathMap.FindOrAdd(ElecPaths[i].GetCurrent()).Add(ElecPaths[i]);
	}
	FElecPath::GenCommonFlag(ElecPaths);
	for (auto it=ElecPathMap.CreateIterator();it;++it)
	{
		FElecTree* tmpElecTree = new FElecTree();
		Internal_GenElecTreeSeries(it->Value, it->Value[0].NexFlag, tmpElecTree);
		TargetTree->Childs.Add(tmpElecTree);
	}
}

void ABattery::Electrify_Implementation(float Voltage)
{
	Super::Electrify_Implementation(Voltage);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Begin Battery Electrify Logic");
	FElecPath SearchMap;
	TArray<FElecPath> ElecPaths;
	//Battery will be the first Elecappliance
	SearchMap.ElecPath.Push(this);
	TArray<FElecLinkInfo> nextLinks = GetNextLinks(PositiveP);
	for (FElecLinkInfo& NextLink:nextLinks)
	{
		Internal_Electrify(NextLink, ElecPaths, SearchMap);
	}
	//TODO: Gen ElecTree
	FElecTree ElecRootTree;
	if (ElecPaths.Num())
	{
		Internal_GenElecTreeSeries(ElecPaths, ElecPaths[0].ElecPath.Num(), &ElecRootTree);
		ElecRootTree.CaculateResistance();
		ElecRootTree.Electrify(this->Voltage);
	}
}

TMap<FString, FQtPropertyInfo> ABattery::CollectSyncableProperty_Implementation()
{
	TMap<FString, FQtPropertyInfo> SuperReturn = AElecappliance::CollectSyncableProperty_Implementation();
	FQtPropertyInfo tmpPropertyInfo;
	tmpPropertyInfo.DisplayName = "Voltage";
	tmpPropertyInfo.Type = EQtPropertyType::QPT_Float;
	tmpPropertyInfo.ValueStr = FString::SanitizeFloat(Voltage);
	SuperReturn.Add("Voltage") = tmpPropertyInfo;
	return SuperReturn;
}

void ABattery::OnPropertyValueChanged_Implementation(const FString& PropertyName, const FString& ValueStr)
{
	Super::OnPropertyValueChanged_Implementation(PropertyName, ValueStr);
	if (PropertyName=="Voltage")
	{
		Voltage = FCString::Atof(*ValueStr);
	}
}

void ABattery::FElecPath::GenCommonFlag(TArray<FElecPath>& ElecPaths)
{
	TArray<AElecappliance*> TargetElecappliance;
	TargetElecappliance.Append(ElecPaths[0].ElecPath.GetData(),ElecPaths[0].ElecPath.Num()-ElecPaths[0].CurIndex);
	for (int i=1;i<ElecPaths.Num();i++)
	{
		TArray<AElecappliance*> tmpElecapplicance;
		ElecPaths[i].PushPin();
		while (!ElecPaths[i].Next())
		{
			if (TargetElecappliance.Find(ElecPaths[i].GetCurrent()) != INDEX_NONE)
				tmpElecapplicance.Add(ElecPaths[i].GetCurrent());
		}
		TargetElecappliance = tmpElecapplicance;
		ElecPaths[i].PopPin();
	}
	for (int i=0;i<ElecPaths.Num();i++)
	{
		ElecPaths[i].NexFlag = ElecPaths[i].ElecPath.Find(TargetElecappliance[0]);
	}
}

void FElecTree::CaculateResistance()
{
	if (Childs.Num() == 0)
	{
		CResistances = Elecappliance->Resistance;
		return;
	}
	float ResistanceCount = 0;
	switch (CircuitType)
	{
		case ECircuitType::Series:
			for (FElecTree*& Tree:Childs)
			{
				Tree->CaculateResistance();
				ResistanceCount += Tree->CResistances;
			}
			CResistances = ResistanceCount;
			break;
		case ECircuitType::Parallel:
			for (FElecTree*& Tree : Childs)
			{
				Tree->CaculateResistance();
				ResistanceCount += 1.f/Tree->CResistances;
			}
			CResistances = 1.f/ResistanceCount;
			break;
		default:
			break;
	}
}

void FElecTree::Electrify(float Voltage)
{
	if (Childs.Num() == 0)
	{
		if (!Elecappliance->GetClass()->IsChildOf(ABattery::StaticClass()))
			Elecappliance->Electrify(Voltage);
		return;
	}
	float ResistanceCount = 0;
	float VSplit;
	switch (CircuitType)
	{
		case ECircuitType::Series:
			for (FElecTree*& Tree : Childs)
				ResistanceCount += Tree->CResistances;
			VSplit = Voltage / ResistanceCount;
			for (FElecTree*& Tree : Childs)
				Tree->Electrify(Tree->CResistances*VSplit);
			break;
		case ECircuitType::Parallel:
			for (FElecTree*& Tree : Childs)
				Tree->Electrify(Voltage);
			break;
		default:
			break;
	}
}
