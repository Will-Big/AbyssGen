#include "PDG/DungeonGenerator.h"
#include "RB_DungeonRoom1.h"
#include "RoomBase.h"
#include "Components/BoxComponent.h"
#include "ClosingWall.h"

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

	FTimerHandle UnusedHandle;

	SpawnStarterRoom();
	SpawnNextRoom();
	this->GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ADungeonGenerator::CloseUnusedExits, 1.0f, false);
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
		AClosingWall* LatestClosingWall = this->GetWorld()->SpawnActor<AClosingWall>(ClosingWall);
		
		FVector RelativeOffset(0.0f, 0.0f, 100.0f);
		FVector WorldOffset = Exit->GetComponentRotation().RotateVector(RelativeOffset);
		
		LatestClosingWall->SetActorLocation(Exit->GetComponentLocation() + WorldOffset);
		LatestClosingWall->SetActorRotation(Exit->GetComponentRotation() + FRotator(0.0f, 90.0f, 0.0f));
	}
}



