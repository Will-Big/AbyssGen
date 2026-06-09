#include "PDG/DungeonGenerator.h"
#include "RoomBase.h"
#include "Components/BoxComponent.h"
#include "ClosingWall.h"
#include "Door.h"
#include "Spawnable.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle ExitsHandle;
	FTimerHandle DoorsHandle;
	FTimerHandle EntitiesHandle;

	SetSeed();

	SpawnStarterRoom();
	SpawnNextRoom();
	SpawnSpecialRooms();
	this->GetWorld()->GetTimerManager().SetTimer(ExitsHandle, this, &ADungeonGenerator::CloseUnusedExits, 1.0f, false);
	this->GetWorld()->GetTimerManager().SetTimer(DoorsHandle, this, &ADungeonGenerator::SpawnDoors, 1.0f, false);
	this->GetWorld()->GetTimerManager().SetTimer(EntitiesHandle, this, &ADungeonGenerator::SpawnEntities, 1.0f, false);
}

void ADungeonGenerator::SetSeed()
{
	int32 Results;
	if (Seed == -1)
	{
		Results = FMath::Rand();
		Seed = Results;
	}
	else
	{
		Results = Seed;
	}
	RandomStream.Initialize(Results);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Seed: %d"), Results));
}

void ADungeonGenerator::SpawnStarterRoom()
{
	ARoomBase* SpawnedStarterRoom = this->GetWorld()->SpawnActor<ARoomBase>(this->StarterRoom);
	SpawnedStarterRoom->SetActorLocation(this->GetActorLocation());
	SpawnedStarterRoom->ExitPointsFolder->GetChildrenComponents(false, this->Exits);

	CollectSpawnPoints(SpawnedStarterRoom);
}

void ADungeonGenerator::SpawnNextRoom()
{
	bCanSpawn = true;

	if (Exits.Num() == 0 || RoomsToBeSpawned.Num() == 0)
	{
		return;
	}

	int32 RoomIndex = RandomStream.RandRange(0, RoomsToBeSpawned.Num() - 1);
	LatestSpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[RoomIndex]);

	int32 ExitIndex = RandomStream.RandRange(0, Exits.Num() - 1);
	USceneComponent* SelectedExitPoint = Exits[ExitIndex];

	LatestSpawnedRoom->SetActorLocation(SelectedExitPoint->GetComponentLocation());
	LatestSpawnedRoom->SetActorRotation(SelectedExitPoint->GetComponentRotation());

	if (IsRoomOverlapping(LatestSpawnedRoom))
	{
		bCanSpawn = false;
		RoomAmount++;
		LatestSpawnedRoom->Destroy();
	}

	if (bCanSpawn)
	{
		DoorList.Add(SelectedExitPoint);
		Exits.Remove(SelectedExitPoint);
		TArray<USceneComponent*> LatestRoomExitPoints;
		LatestSpawnedRoom->ExitPointsFolder->GetChildrenComponents(false, LatestRoomExitPoints);
		Exits.Append(LatestRoomExitPoints);

		CollectSpawnPoints(LatestSpawnedRoom);
	}

	RoomAmount--;
	if (RoomAmount > 0)
	{
		SpawnNextRoom();
	}
}

void ADungeonGenerator::SpawnSpecialRooms()
{
	if (SpecialRoomsToBeSpawned.Num() == 0 || SpecialRoomAmount <= 0)
	{
		return;
	}

	for (int32 Placed = 0; Placed < SpecialRoomAmount; ++Placed)
	{
		if (Exits.Num() == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Special room placement stopped: no remaining exits"));
			break;
		}

		TArray<USceneComponent*> Candidates = Exits;
		bool bPlaced = false;

		while (Candidates.Num() > 0 && !bPlaced)
		{
			int32 CandIndex = RandomStream.RandRange(0, Candidates.Num() - 1);
			USceneComponent* Exit = Candidates[CandIndex];
			Candidates.RemoveAt(CandIndex);

			int32 ClassIndex = RandomStream.RandRange(0, SpecialRoomsToBeSpawned.Num() - 1);
			ARoomBase* SpecialRoom = this->GetWorld()->SpawnActor<ARoomBase>(SpecialRoomsToBeSpawned[ClassIndex]);
			SpecialRoom->SetActorLocation(Exit->GetComponentLocation());
			SpecialRoom->SetActorRotation(Exit->GetComponentRotation());

			if (IsRoomOverlapping(SpecialRoom))
			{
				SpecialRoom->Destroy();
				continue;
			}

			DoorList.Add(Exit);
			Exits.Remove(Exit);
			CollectSpawnPoints(SpecialRoom);
			bPlaced = true;
		}

		if (!bPlaced)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Special room placement failed: no non-overlapping exit"));
		}
	}
}

bool ADungeonGenerator::IsRoomOverlapping(ARoomBase* Room) const
{
	if (!Room)
	{
		return false;
	}

	TArray<USceneComponent*> OverlapBoxes;
	Room->OverlapFolder->GetChildrenComponents(false, OverlapBoxes);

	for (USceneComponent* OverlapBox : OverlapBoxes)
	{
		UBoxComponent* Box = Cast<UBoxComponent>(OverlapBox);
		if (!Box)
		{
			continue;
		}

		TArray<UPrimitiveComponent*> OverlappingComponents;
		Box->GetOverlappingComponents(OverlappingComponents);

		for (UPrimitiveComponent* OverlappingComponent : OverlappingComponents)
		{
			if (OverlappingComponent->GetOwner() != Room)
			{
				return true;
			}
		}
	}

	return false;
}

void ADungeonGenerator::CloseUnusedExits()
{
	for (USceneComponent* Exit : Exits)
	{
		AClosingWall* LatestClosingWall = this->GetWorld()->SpawnActor<AClosingWall>(ClosingWall);

		FVector RelativeOffset(0.0f, 0.0f, 100.0f);
		FVector WorldOffset = Exit->GetComponentRotation().RotateVector(RelativeOffset);

		LatestClosingWall->SetActorLocation(Exit->GetComponentLocation() + WorldOffset);
		LatestClosingWall->SetActorRotation(Exit->GetComponentRotation() + FRotator(0.0f, 90.0f, 0.0f));
	}
}

void ADungeonGenerator::SpawnDoors()
{
	for (USceneComponent* NewDoor : DoorList)
	{
		ADoor* LatestDoorSpawned = this->GetWorld()->SpawnActor<ADoor>(Door);

		FVector RelativeOffset(0.0f, 0.0f, 300.0f);
		FVector WorldOffset = NewDoor->GetComponentRotation().RotateVector(RelativeOffset);

		LatestDoorSpawned->SetActorLocation(NewDoor->GetComponentLocation() + WorldOffset);
		LatestDoorSpawned->SetActorRotation(NewDoor->GetComponentRotation() + FRotator(0.0f, 90.0f, 0.0f));
	}
}

void ADungeonGenerator::CollectSpawnPoints(AActor* Room)
{
	if (!Room)
	{
		return;
	}

	TArray<USpawnPointComponent*> FoundPoints;
	Room->GetComponents(FoundPoints);
	SpawnPoints.Append(FoundPoints);
}

void ADungeonGenerator::SpawnEntities()
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

		if (!ShouldSpawnPoint(Point, *Table))
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

const FSpawnTable* ADungeonGenerator::ResolveSpawnTable(const USpawnPointComponent* Point) const
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

bool ADungeonGenerator::ShouldSpawnPoint(const USpawnPointComponent* Point, const FSpawnTable& Table)
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

void ADungeonGenerator::SpawnActorAtPoint(const USpawnPointComponent* Point, TSubclassOf<AActor> ActorClass)
{
	if (!Point || !ActorClass)
	{
		return;
	}

	FVector SpawnLocation = Point->GetComponentLocation();

	if (Point->bSnapToFloor)
	{
		const FVector TraceStart = SpawnLocation + FVector(0.0f, 0.0f, 100.0f);
		const FVector TraceEnd = SpawnLocation - FVector(0.0f, 0.0f, 2000.0f);

		FHitResult Hit;
		if (this->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic))
		{
			SpawnLocation = Hit.Location;
		}
	}

	AActor* DefaultActor = ActorClass.GetDefaultObject();
	if (DefaultActor && DefaultActor->Implements<USpawnable>())
	{
		SpawnLocation.Z += ISpawnable::Execute_GetGroundOffset(DefaultActor);
	}

	this->GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, Point->GetComponentRotation());
}
