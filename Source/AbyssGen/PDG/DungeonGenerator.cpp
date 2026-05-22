#include "PDG/DungeonGenerator.h"
#include "RB_StarterRoom.h"
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
	ARB_StarterRoom* SpawnedStarterRoom = this->GetWorld()->SpawnActor<ARB_StarterRoom>(this->StarterRoom);
	SpawnedStarterRoom->SetActorLocation(this->GetActorLocation());
	SpawnedStarterRoom->ExitPointsFolder->GetChildrenComponents(false, this->Exits);

	CollectSpawnPoints(SpawnedStarterRoom);
}

void ADungeonGenerator::SpawnNextRoom()
{
	bCanSpawn = true;

	int32 RoomIndex = RandomStream.RandRange(0, RoomsToBeSpawned.Num() - 1);
	LatestSpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[RoomIndex]);

	int32 ExitIndex = RandomStream.RandRange(0, Exits.Num() - 1);
	USceneComponent* SelectedExitPoint = Exits[ExitIndex];

	LatestSpawnedRoom->SetActorLocation(SelectedExitPoint->GetComponentLocation());
	LatestSpawnedRoom->SetActorRotation(SelectedExitPoint->GetComponentRotation());

	DoorList.Add(SelectedExitPoint);

	RemoveOverlappingRooms();

	if (bCanSpawn)
	{
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

void ADungeonGenerator::RemoveOverlappingRooms()
{
	TArray<USceneComponent*> OverlappedRooms;
	LatestSpawnedRoom->OverlapFolder->GetChildrenComponents(false, OverlappedRooms);
	
	TArray<UPrimitiveComponent*> OverlappingComponents;
	for (USceneComponent* OverlappedRoom : OverlappedRooms)
	{
		Cast<UBoxComponent>(OverlappedRoom)->GetOverlappingComponents(OverlappingComponents);
	}

	for (UPrimitiveComponent* OverlappingComponent : OverlappingComponents)
	{
		bCanSpawn = false;
		RoomAmount++;
		LatestSpawnedRoom->Destroy();
	}
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
	for (USpawnPointComponent* Point : SpawnPoints)
	{
		if (!Point)
		{
			continue;
		}

		const FSpawnTable* Table = SpawnTables.Find(Point->Category);
		if (!Table || Table->Classes.Num() == 0)
		{
			continue;
		}

		// 포인트별 확률 굴림 — 탈락하면 이 포인트는 비움
		if (RandomStream.FRand() > Table->SpawnChance)
		{
			continue;
		}

		const TSubclassOf<AActor> ActorClass = Table->Classes[RandomStream.RandRange(0, Table->Classes.Num() - 1)];
		if (!ActorClass)
		{
			continue;
		}

		FVector SpawnLocation = Point->GetComponentLocation();

		// 바닥 스냅 — 아래로 라인트레이스해 바닥 표면을 찾음
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

		// 요소별 바닥 오프셋 — 스폰 클래스가 ISpawnable이면 적용
		AActor* DefaultActor = ActorClass.GetDefaultObject();
		if (DefaultActor && DefaultActor->Implements<USpawnable>())
		{
			SpawnLocation.Z += ISpawnable::Execute_GetGroundOffset(DefaultActor);
		}

		this->GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, Point->GetComponentRotation());
	}
}



