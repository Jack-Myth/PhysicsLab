// Fill out your copyright notice in the Description page of Project Settings.

#include "Battery.h"

void ABattery::Internal_Electrify(FElecLinkInfo BeginSearch, TArray<FElecPath>& ElecPaths, FElecPath& SearchMap)
{
	if (SearchMap.ElecPath.Find(BeginSearch.Elecappliance) != INDEX_NONE)
		return;
	SearchMap.ElecPath.Push(BeginSearch.Elecappliance);
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

void ABattery::Electrify_Implementation(float Voltage)
{
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
}
