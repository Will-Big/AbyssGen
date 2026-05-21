// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

class ARB_DungeonRoom1;
class ARoomBase;
class AClosingWall;
class ADoor;

UCLASS()
class ABYSSGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADungeonGenerator();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TSubclassOf<ARB_DungeonRoom1> StarterRoom;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	UPROPERTY(EditAnywhere, Category = "Unused Exits Closing Wall")
	TSubclassOf<AClosingWall> ClosingWall;

	UPROPERTY(EditAnywhere, Category = "Door")
	TSubclassOf<ADoor> Door;

	UPROPERTY(EditAnywhere, Category = "Dungeon Info")
	int32 RoomAmount;

	ARoomBase* LatestSpawnedRoom;

	bool bCanSpawn;

	TArray<USceneComponent*> Exits;

	TArray<USceneComponent*> DoorList;

	FRandomStream RandomStream;

	UPROPERTY(EditAnywhere, Category = "Dungeon Info")
	int32 Seed;

	void SetSeed();

	void SpawnStarterRoom();

	void SpawnNextRoom();

	void RemoveOverlappingRooms();

	void CloseUnusedExits();

	void SpawnDoors();
};
