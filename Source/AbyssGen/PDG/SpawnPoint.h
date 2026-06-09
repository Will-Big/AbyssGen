// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Templates/SubclassOf.h"
#include "SpawnPoint.generated.h"

UENUM(BlueprintType)
enum class ESpawnContentType : uint8
{
	Enemy UMETA(DisplayName = "Enemy"),
	Item UMETA(DisplayName = "Item"),
	Reward UMETA(DisplayName = "Reward"),
	Prop UMETA(DisplayName = "Prop"),
	Trap UMETA(DisplayName = "Trap"),
	Objective UMETA(DisplayName = "Objective")
};

UENUM(BlueprintType)
enum class ESpawnRequirement : uint8
{
	Optional UMETA(DisplayName = "Optional"),
	Guaranteed UMETA(DisplayName = "Guaranteed"),
	Unique UMETA(DisplayName = "Unique"),
	RoomUnique UMETA(DisplayName = "Room Unique")
};

USTRUCT(BlueprintType)
struct FSpawnTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<TSubclassOf<AActor>> Classes;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnChance = 1.0f;
};

UCLASS(ClassGroup = (Dungeon), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API USpawnPointComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	ESpawnContentType ContentType = ESpawnContentType::Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	ESpawnRequirement Requirement = ESpawnRequirement::Optional;

	// Optional table key such as Enemy.Basic, Enemy.Elite, or Reward.Special.
	// If empty, the generator uses ContentType's default table.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	FName SpawnTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	bool bSnapToFloor = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Point")
	bool bSpawnOnDungeonGeneration = true;
};
