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
	bool bRandomizeSeed = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	int32 Seed = 42;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LoopRatio = 0.15f;

	UFUNCTION(CallInEditor, Category = "Dungeon")
	void Generate();

	UFUNCTION(CallInEditor, Category = "Dungeon")
	void StepSeparation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	int32 TotalRoomCount = 80;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	float SpawnRadius = 40.f; // 칸 단위

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	int32 MaxSeparationIterations = 500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	float SeparationPadding = 1.f; // 방 사이 최소 칸 간격

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon")
	float MainRoomSizeMultiplier = 1.25f;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Dungeon|Debug")
	FDungeonGraph DungeonGraph;

private:
	void GenerateRooms(FRandomStream& Rand);
	int32 SeparationStep();
	int32 RunSeparation();
	void SelectMainRooms();
	void Triangulate();
	void DeduplicateEdges();
	void BuildMST();
	void ReviveLoops(FRandomStream& Rand);
	void ComputeCorridors(FRandomStream& Rand);
	void MarkDoorCells();
	void AssignSpecialRooms(FRandomStream& Rand);
	void VerifyConnectivity() const;
};
