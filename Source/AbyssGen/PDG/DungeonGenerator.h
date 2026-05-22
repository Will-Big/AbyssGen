// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.h"
#include "DungeonGenerator.generated.h"

class ARB_StarterRoom;
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
	TSubclassOf<ARB_StarterRoom> StarterRoom;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	UPROPERTY(EditAnywhere, Category = "Unused Exits Closing Wall")
	TSubclassOf<AClosingWall> ClosingWall;

	UPROPERTY(EditAnywhere, Category = "Door")
	TSubclassOf<ADoor> Door;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TMap<ESpawnCategory, FSpawnTable> SpawnTables;

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

	/** 생성된 방들에서 수집한 스폰 포인트 */
	TArray<USpawnPointComponent*> SpawnPoints;

	/** 방 액터에서 USpawnPointComponent를 모아 SpawnPoints에 누적 */
	void CollectSpawnPoints(AActor* Room);

	/** 수집된 스폰 포인트마다 확률을 굴려 액터를 스폰 */
	void SpawnEntities();
};
