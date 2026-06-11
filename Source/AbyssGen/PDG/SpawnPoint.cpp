// Fill out your copyright notice in the Description page of Project Settings.

#include "PDG/SpawnPoint.h"
#include "Spawnable.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

FVector USpawnPointComponent::ResolveGroundedSpawnLocation(
	const UWorld* World,
	const FVector& BaseLocation,
	TSubclassOf<AActor> ActorClass,
	bool bSnapToFloor,
	float TraceHeight,
	float TraceDepth)
{
	FVector SpawnLocation = BaseLocation;

	if (bSnapToFloor && World)
	{
		const FVector TraceStart = SpawnLocation + FVector(0.0f, 0.0f, TraceHeight);
		const FVector TraceEnd = SpawnLocation - FVector(0.0f, 0.0f, TraceDepth);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic))
		{
			SpawnLocation = Hit.Location;
		}
	}

	AActor* DefaultActor = ActorClass.GetDefaultObject();
	if (DefaultActor && DefaultActor->Implements<USpawnable>())
	{
		SpawnLocation.Z += ISpawnable::Execute_GetGroundOffset(DefaultActor);
	}

	return SpawnLocation;
}
