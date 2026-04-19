#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.generated.h"

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Normal,
	Start,
	Boss,
	Treasure
};

USTRUCT(BlueprintType)
struct FRoom
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint GridCenter = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint GridSize = FIntPoint(6, 6);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D FloatCenter = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMain = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ERoomType Type = ERoomType::Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FIntPoint> DoorCells;
};

USTRUCT(BlueprintType)
struct FDungeonEdge
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RoomA = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RoomB = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Weight = 0.f;
};

USTRUCT(BlueprintType)
struct FLCorridor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint Start = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint Corner = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint End = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RoomA = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RoomB = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FDungeonGeneratorConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRandomizeSeed = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Seed = 42;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LoopRatio = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TotalRoomCount = 80;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnRadius = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxSeparationIterations = 500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SeparationPadding = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MainRoomSizeMultiplier = 1.25f;
};

USTRUCT(BlueprintType)
struct FDungeonGraph
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRoom> Rooms;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> MainRoomIndices;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FDungeonEdge> DelaunayEdges;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FDungeonEdge> MstEdges;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FDungeonEdge> LoopEdges;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FLCorridor> Corridors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Seed = 0;

	void Reset()
	{
		Rooms.Reset();
		MainRoomIndices.Reset();
		DelaunayEdges.Reset();
		MstEdges.Reset();
		LoopEdges.Reset();
		Corridors.Reset();
		Seed = 0;
	}
};
