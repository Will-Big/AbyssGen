#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.generated.h"

namespace AbyssGen::Dungeon
{
	constexpr float CellSizeUU = 100.f;
}

UENUM(BlueprintType)
enum class EDungeonRoomType : uint8
{
	Normal,
	Start,
	Boss,
	Treasure
};

USTRUCT(BlueprintType)
struct FDungeonRoom
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
	FVector2D Center = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
	FIntPoint Size = FIntPoint(6, 6);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
	bool bIsMain = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
	EDungeonRoomType Type = EDungeonRoomType::Normal;
};

USTRUCT(BlueprintType)
struct FDungeonEdge
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edge")
	int32 RoomA = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edge")
	int32 RoomB = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edge")
	float Length = 0.f;
};

/**
 * L자 복도 한 개. 칸 좌표 3점(시작-꺾임-끝)으로 표현.
 * 실제 칸 목록(시작→꺾임 직선 + 꺾임→끝 직선)은 시각화/스폰 단계에서 전개.
 */
USTRUCT(BlueprintType)
struct FLCorridor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Corridor")
	FIntPoint Start = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Corridor")
	FIntPoint Corner = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Corridor")
	FIntPoint End = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Corridor")
	int32 RoomA = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Corridor")
	int32 RoomB = INDEX_NONE;
};
