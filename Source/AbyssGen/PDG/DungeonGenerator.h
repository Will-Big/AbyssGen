// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

class ARoomBase;
class AEscapeRoom;
class AClosingWall;
class ADoor;
class USceneComponent;
class UDungeonPopulatorComponent;

UCLASS()
class ABYSSGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADungeonGenerator();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TSubclassOf<ARoomBase> StarterRoom;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> SpecialRoomsToBeSpawned;

	/** 던전마다 정확히 하나 배치할 탈출방 클래스 */
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TSubclassOf<AEscapeRoom> EscapeRoomClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Rooms")
	TObjectPtr<AEscapeRoom> SpawnedEscapeRoom;

	/** 던전당 배치를 보장할 특수방 개수 */
	UPROPERTY(EditAnywhere, Category = "Rooms")
	int32 SpecialRoomAmount;

	UPROPERTY(EditAnywhere, Category = "Unused Exits Closing Wall")
	TSubclassOf<AClosingWall> ClosingWall;

	UPROPERTY(EditAnywhere, Category = "Door")
	TSubclassOf<ADoor> Door;

	/** 연결된 출구(문) 방향에 생성할 토치 */
	UPROPERTY(EditAnywhere, Category = "Torch")
	TSubclassOf<AActor> Torch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	TObjectPtr<UDungeonPopulatorComponent> PopulatorComponent;

	UPROPERTY(EditAnywhere, Category = "Dungeon Info")
	int32 RoomAmount;

	TArray<USceneComponent*> Exits;

	TArray<USceneComponent*> DoorList;

	FRandomStream RandomStream;

	UPROPERTY(EditAnywhere, Category = "Dungeon Info")
	int32 Seed;

	void SetSeed();

	void SpawnStarterRoom();

	void SpawnNextRoom();

	/** 남은 출구 중 생성기에서 먼 곳부터 시도해 탈출방을 정확히 하나 배치한다. */
	bool SpawnEscapeRoom();

	/** 일반 방 배치 후 남은 출구에 막다른 특수방을 SpecialRoomAmount개 배치 */
	void SpawnSpecialRooms();

	/** Room의 겹침 박스가 다른 액터와 겹치는지만 검사 (부수효과 없음) */
	bool IsRoomOverlapping(ARoomBase* Room) const;

	void CloseUnusedExits();

	void SpawnDoors();

	/** 연결된 출구(DoorList)의 Arrow 아래 미리 배치된 스폰포인트 위치에 토치를 생성 */
	void SpawnTorches();

	/** 지정한 지점들마다 액터를 스폰해 (회전 기준 ZOffset + 90도 yaw)로 배치 */
	void SpawnActorsAtPoints(const TArray<USceneComponent*>& Points, TSubclassOf<AActor> ActorClass, float ZOffset);

	/** 레이아웃 완료 후 PopulatorComponent로 콘텐츠 배치를 위임 */
	void BeginPopulate();

	/** 레이아웃 완료 후 다음 틱에 벽/문/엔티티 배치를 마감 */
	void FinalizeDungeon();
};
