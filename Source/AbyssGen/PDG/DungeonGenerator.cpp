#include "PDG/DungeonGenerator.h"
#include "RB_DungeonRoom1.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADungeonGenerator::SpawnStarterRoom()
{
	ARB_DungeonRoom1* SpawnedStarterRoom = this->GetWorld()->SpawnActor<ARB_DungeonRoom1>(this->StarterRoom);
	SpawnedStarterRoom->SetActorLocation(this->GetActorLocation());
	SpawnedStarterRoom->ExitPointsFolder->GetChildrenComponents(false, this->Exits);
}

void ADungeonGenerator::SpawnNextRoom()
{
	ARoomBase* LatestSpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[rand() % RoomsToBeSpawned.Num()]);

	USceneComponent* SelectedExitPoint = Exits[rand() % Exits.Num()];

	LatestSpawnedRoom->SetActorLocation(SelectedExitPoint->GetComponentLocation());
	LatestSpawnedRoom->SetActorRotation(SelectedExitPoint->GetComponentRotation());
}

void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnStarterRoom();
	SpawnNextRoom();
}


