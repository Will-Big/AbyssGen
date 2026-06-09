// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.h"
#include "DungeonGenerator.generated.h"

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
	TSubclassOf<ARoomBase> StarterRoom;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> SpecialRoomsToBeSpawned;

	/** 던전당 배치를 보장할 특수방 개수 */
	UPROPERTY(EditAnywhere, Category = "Rooms")
	int32 SpecialRoomAmount;

	UPROPERTY(EditAnywhere, Category = "Unused Exits Closing Wall")
	TSubclassOf<AClosingWall> ClosingWall;

	UPROPERTY(EditAnywhere, Category = "Door")
	TSubclassOf<ADoor> Door;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TMap<ESpawnContentType, FSpawnTable> DefaultSpawnTables;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TMap<FName, FSpawnTable> TaggedSpawnTables;

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

	/** 일반 방 배치 후 남은 출구에 막다른 특수방을 SpecialRoomAmount개 배치 */
	void SpawnSpecialRooms();

	/** Room의 겹침 박스가 다른 액터와 겹치는지만 검사 (부수효과 없음) */
	bool IsRoomOverlapping(ARoomBase* Room) const;

	void CloseUnusedExits();

	void SpawnDoors();

	/** 생성된 방들에서 수집한 스폰 포인트 */
	TArray<USpawnPointComponent*> SpawnPoints;

	/** 방 액터에서 USpawnPointComponent를 모아 SpawnPoints에 누적 */
	void CollectSpawnPoints(AActor* Room);

	/** 수집된 스폰 포인트마다 확률을 굴려 액터를 스폰 */
	void SpawnEntities();

	const FSpawnTable* ResolveSpawnTable(const USpawnPointComponent* Point) const;

	bool ShouldSpawnPoint(const USpawnPointComponent* Point, const FSpawnTable& Table);

	void SpawnActorAtPoint(const USpawnPointComponent* Point, TSubclassOf<AActor> ActorClass);
};
