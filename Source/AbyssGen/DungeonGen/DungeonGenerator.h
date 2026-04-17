#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonTypes.h"
#include "DungeonGenerator.generated.h"

UCLASS()
class ABYSSGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	int32 Seed = 42;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LoopRatio = 0.15f;

	UFUNCTION(CallInEditor, Category = "Dungeon")
	void Generate();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	int32 TotalRoomCount = 80;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	float SpawnRadius = 40.f; // 칸 단위

protected:
	UPROPERTY(VisibleAnywhere, Category = "Dungeon|Debug")
	FDungeonGraph DungeonGraph;

private:
	void GenerateRooms(FRandomStream& Rand);
};
