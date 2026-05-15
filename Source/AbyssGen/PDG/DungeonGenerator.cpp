#include "PDG/DungeonGenerator.h"
#include "RB_DungeonRoom1.h"
#include "RoomBase.h"
#include "Components/BoxComponent.h"

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

	SpawnStarterRoom();
	SpawnNextRoom();
	CloseUnusedExits();
}

void ADungeonGenerator::SpawnStarterRoom()
{
	ARB_DungeonRoom1* SpawnedStarterRoom = this->GetWorld()->SpawnActor<ARB_DungeonRoom1>(this->StarterRoom);
	SpawnedStarterRoom->SetActorLocation(this->GetActorLocation());
	SpawnedStarterRoom->ExitPointsFolder->GetChildrenComponents(false, this->Exits);
}

void ADungeonGenerator::SpawnNextRoom()
{
	bCanSpawn = true;

	LatestSpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[rand() % RoomsToBeSpawned.Num()]);

	USceneComponent* SelectedExitPoint = Exits[rand() % Exits.Num()];

	LatestSpawnedRoom->SetActorLocation(SelectedExitPoint->GetComponentLocation());
	LatestSpawnedRoom->SetActorRotation(SelectedExitPoint->GetComponentRotation());

	RemoveOverlappingRooms();

	if (bCanSpawn)
	{
		Exits.Remove(SelectedExitPoint);
		TArray<USceneComponent*> LatestRoomExitPoints;
		LatestSpawnedRoom->ExitPointsFolder->GetChildrenComponents(false, LatestRoomExitPoints);
		Exits.Append(LatestRoomExitPoints);
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
		Exit->SetVisibility(false);
	}
}



