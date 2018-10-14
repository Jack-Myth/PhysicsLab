// Fill out your copyright notice in the Description page of Project Settings.

#include "Battery.h"

void ABattery::Internal_Electrify(FElecLinkInfo BeginSearch, TArray<TArray<AElecappliance*>>& ElecPaths,TArray<AElecappliance*>& SearchMap)
{
	if (SearchMap.Find(BeginSearch.Elecappliance) != INDEX_NONE)
		return;
	SearchMap.Push(BeginSearch.Elecappliance);
	TArray<FElecLinkInfo> nextLinks = BeginSearch.Elecappliance->GetNextLinks(BeginSearch.ExitPole);
	for (FElecLinkInfo& NextLink : nextLinks)
	{
		if (NextLink.Elecappliance==this)
		{
			if (NextLink.ExitPole != NegativeP)
			{
				SearchMap.Push(this);
				ElecPaths.Add(SearchMap);
				SearchMap.Pop();
			}
			continue;
		}
		Internal_Electrify(NextLink,ElecPaths, SearchMap);
	}
	SearchMap.Pop();
}

void ABattery::Electrify_Implementation(float Voltage)
{
	TArray<AElecappliance*> SearchMap;
	TArray<TArray<AElecappliance*>> ElecPaths;
	//Battery will be the first Elecappliance
	SearchMap.Push(this);
	TArray<FElecLinkInfo> nextLinks = GetNextLinks(PositiveP);
	for (FElecLinkInfo& NextLink:nextLinks)
	{
		Internal_Electrify(NextLink, ElecPaths, SearchMap);
	}
}
