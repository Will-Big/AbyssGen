// Fill out your copyright notice in the Description page of Project Settings.

#include "PDG/DungeonPopulatorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UDungeonPopulatorComponent::UDungeonPopulatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDungeonPopulatorComponent::CollectFrom(AActor* Room)
{
	if (!Room)
	{
		return;
	}

	TArray<USpawnPointComponent*> FoundPoints;
	Room->GetComponents(FoundPoints);

	for (USpawnPointComponent* Point : FoundPoints)
	{
		SpawnPoints.Add(Point);
	}
}

void UDungeonPopulatorComponent::Populate(FRandomStream& RandomStream)
{
	TSet<FName> SpawnedUniqueTags;
	TSet<FString> SpawnedRoomUniqueKeys;

	for (USpawnPointComponent* Point : SpawnPoints)
	{
		if (!Point)
		{
			continue;
		}

		if (!Point->bSpawnOnDungeonGeneration)
		{
			continue;
		}

		const FSpawnTable* Table = ResolveSpawnTable(Point);
		if (!Table || Table->Classes.Num() == 0)
		{
			continue;
		}

		const FName SpawnKey = Point->SpawnTag.IsNone()
			? FName(*UEnum::GetValueAsString(Point->ContentType))
			: Point->SpawnTag;

		if (Point->Requirement == ESpawnRequirement::Unique)
		{
			if (SpawnedUniqueTags.Contains(SpawnKey))
			{
				continue;
			}
		}
		else if (Point->Requirement == ESpawnRequirement::RoomUnique)
		{
			const AActor* RoomOwner = Point->GetOwner();
			const FString RoomKey = FString::Printf(TEXT("%s:%s"), *GetNameSafe(RoomOwner), *SpawnKey.ToString());
			if (SpawnedRoomUniqueKeys.Contains(RoomKey))
			{
				continue;
			}
		}

		if (!ShouldSpawnPoint(Point, *Table, RandomStream))
		{
			continue;
		}

		const TSubclassOf<AActor> ActorClass = Table->Classes[RandomStream.RandRange(0, Table->Classes.Num() - 1)];
		if (!ActorClass)
		{
			continue;
		}

		SpawnActorAtPoint(Point, ActorClass);

		if (Point->Requirement == ESpawnRequirement::Unique)
		{
			SpawnedUniqueTags.Add(SpawnKey);
		}
		else if (Point->Requirement == ESpawnRequirement::RoomUnique)
		{
			const AActor* RoomOwner = Point->GetOwner();
			const FString RoomKey = FString::Printf(TEXT("%s:%s"), *GetNameSafe(RoomOwner), *SpawnKey.ToString());
			SpawnedRoomUniqueKeys.Add(RoomKey);
		}
	}
}

const FSpawnTable* UDungeonPopulatorComponent::ResolveSpawnTable(const USpawnPointComponent* Point) const
{
	if (!Point)
	{
		return nullptr;
	}

	if (!Point->SpawnTag.IsNone())
	{
		if (const FSpawnTable* TaggedTable = TaggedSpawnTables.Find(Point->SpawnTag))
		{
			return TaggedTable;
		}
	}

	return DefaultSpawnTables.Find(Point->ContentType);
}

bool UDungeonPopulatorComponent::ShouldSpawnPoint(const USpawnPointComponent* Point, const FSpawnTable& Table, FRandomStream& RandomStream) const
{
	if (!Point)
	{
		return false;
	}

	if (Point->Requirement != ESpawnRequirement::Optional)
	{
		return true;
	}

	const float CombinedChance = FMath::Clamp(Point->SpawnChance * Table.SpawnChance, 0.0f, 1.0f);
	return RandomStream.FRand() <= CombinedChance;
}

void UDungeonPopulatorComponent::SpawnActorAtPoint(const USpawnPointComponent* Point, TSubclassOf<AActor> ActorClass)
{
	if (!Point || !ActorClass)
	{
		return;
	}

	const FVector SpawnLocation = USpawnPointComponent::ResolveGroundedSpawnLocation(
		GetWorld(), Point->GetComponentLocation(), ActorClass, Point->bSnapToFloor);

	GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, Point->GetComponentRotation());
}
