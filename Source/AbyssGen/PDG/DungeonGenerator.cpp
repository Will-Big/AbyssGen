#include "PDG/DungeonGenerator.h"
#include "RoomBase.h"
#include "Components/BoxComponent.h"
#include "ClosingWall.h"
#include "Door.h"
#include "PDG/DungeonPopulatorComponent.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	PopulatorComponent = CreateDefaultSubobject<UDungeonPopulatorComponent>(TEXT("PopulatorComponent"));
}

void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();

	SetSeed();

	SpawnStarterRoom();
	SpawnNextRoom();
	SpawnSpecialRooms();

	// 레이아웃은 동기적으로 끝나지만, 스폰된 방들의 콜리전 오버랩이 다음 틱에 정리되므로
	// 마감 단계(벽/문/엔티티 배치)는 다음 틱에 정해진 순서의 단일 콜백으로 실행한다.
	GetWorldTimerManager().SetTimerForNextTick(this, &ADungeonGenerator::FinalizeDungeon);
}

void ADungeonGenerator::FinalizeDungeon()
{
	CloseUnusedExits();
	SpawnDoors();
	SpawnTorches();
	BeginPopulate();
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
	ARoomBase* SpawnedStarterRoom = GetWorld()->SpawnActor<ARoomBase>(StarterRoom);
	SpawnedStarterRoom->SetActorLocation(GetActorLocation());
	SpawnedStarterRoom->ExitPointsFolder->GetChildrenComponents(false, Exits);

	PopulatorComponent->CollectFrom(SpawnedStarterRoom);
}

void ADungeonGenerator::SpawnNextRoom()
{
	if (Exits.Num() == 0 || RoomsToBeSpawned.Num() == 0)
	{
		return;
	}

	const int32 RoomIndex = RandomStream.RandRange(0, RoomsToBeSpawned.Num() - 1);
	ARoomBase* NewRoom = GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[RoomIndex]);

	USceneComponent* SelectedExit = Exits[RandomStream.RandRange(0, Exits.Num() - 1)];
	NewRoom->SetActorLocationAndRotation(SelectedExit->GetComponentLocation(), SelectedExit->GetComponentRotation());

	if (IsRoomOverlapping(NewRoom))
	{
		// 겹치면 폐기하고 같은 횟수로 재시도
		NewRoom->Destroy();
		RoomAmount++;
	}
	else
	{
		DoorList.Add(SelectedExit);
		Exits.Remove(SelectedExit);

		TArray<USceneComponent*> NewRoomExits;
		NewRoom->ExitPointsFolder->GetChildrenComponents(false, NewRoomExits);
		Exits.Append(NewRoomExits);

		PopulatorComponent->CollectFrom(NewRoom);
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
			ARoomBase* SpecialRoom = GetWorld()->SpawnActor<ARoomBase>(SpecialRoomsToBeSpawned[ClassIndex]);
			SpecialRoom->SetActorLocation(Exit->GetComponentLocation());
			SpecialRoom->SetActorRotation(Exit->GetComponentRotation());

			if (IsRoomOverlapping(SpecialRoom))
			{
				SpecialRoom->Destroy();
				continue;
			}

			DoorList.Add(Exit);
			Exits.Remove(Exit);
			PopulatorComponent->CollectFrom(SpecialRoom);
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
	SpawnActorsAtPoints(Exits, ClosingWall, 100.0f);
}

void ADungeonGenerator::SpawnDoors()
{
	SpawnActorsAtPoints(DoorList, Door, 300.0f);
}

void ADungeonGenerator::SpawnTorches()
{
	if (!Torch)
	{
		return;
	}

	// 연결된 출구(문)의 Arrow마다, 그 아래 미리 배치된 스폰포인트 위치/회전에 토치를 생성
	for (USceneComponent* Exit : DoorList)
	{
		if (!Exit)
		{
			continue;
		}

		TArray<USceneComponent*> TorchPoints;
		Exit->GetChildrenComponents(false, TorchPoints);

		for (USceneComponent* Point : TorchPoints)
		{
			if (!Point)
			{
				continue;
			}

			GetWorld()->SpawnActor<AActor>(Torch, Point->GetComponentLocation(), Point->GetComponentRotation());
		}
	}
}

void ADungeonGenerator::SpawnActorsAtPoints(const TArray<USceneComponent*>& Points, TSubclassOf<AActor> ActorClass, float ZOffset)
{
	if (!ActorClass)
	{
		return;
	}

	for (USceneComponent* Point : Points)
	{
		if (!Point)
		{
			continue;
		}

		AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorClass);
		if (!Spawned)
		{
			continue;
		}

		const FRotator PointRotation = Point->GetComponentRotation();
		const FVector WorldOffset = PointRotation.RotateVector(FVector(0.0f, 0.0f, ZOffset));

		Spawned->SetActorLocation(Point->GetComponentLocation() + WorldOffset);
		Spawned->SetActorRotation(PointRotation + FRotator(0.0f, 90.0f, 0.0f));
	}
}

void ADungeonGenerator::BeginPopulate()
{
	if (PopulatorComponent)
	{
		PopulatorComponent->Populate(RandomStream);
	}
}
