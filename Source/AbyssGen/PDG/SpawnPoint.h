// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Templates/SubclassOf.h"
#include "SpawnPoint.generated.h"

/** 스폰 포인트가 어떤 종류의 액터를 위한 자리인지 */
UENUM(BlueprintType)
enum class ESpawnCategory : uint8
{
	Enemy UMETA(DisplayName = "Enemy"),
	Item  UMETA(DisplayName = "Item"),
	Prop  UMETA(DisplayName = "Prop")
};

/** 한 카테고리의 스폰 후보 클래스와 스폰 확률 */
USTRUCT(BlueprintType)
struct FSpawnTable
{
	GENERATED_BODY()

	/** 이 카테고리에서 스폰 후보가 되는 액터 클래스들 */
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<TSubclassOf<AActor>> Classes;

	/** 스폰 포인트 하나가 실제로 스폰될 확률 (0~1) */
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnChance = 0.5f;
};

/**
 *  방 BP에 배치하는 스폰 위치 마커.
 *  DungeonGenerator가 수집해 카테고리별로 액터를 스폰한다.
 */
UCLASS(ClassGroup = (Dungeon), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API USpawnPointComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/** 이 스폰 포인트의 용도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	ESpawnCategory Category = ESpawnCategory::Enemy;

	/** true면 스폰 시 바닥으로 라인트레이스해 위치를 보정. false면 마커 위치 그대로 (벽 부착물용) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	bool bSnapToFloor = true;
};
