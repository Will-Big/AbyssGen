#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.h"

class ABYSSGEN_API FDungeonDebugDraw
{
public:
	static void DrawRoom(const UWorld* World, const FRoom& Room, FColor Color = FColor::White, float Duration = 10.f);
	static void DrawEdge(const UWorld* World, const FDungeonEdge& Edge, const TArray<FRoom>& Rooms, FColor Color = FColor::Cyan, float Duration = 10.f);
	static void DrawCorridor(const UWorld* World, const FLCorridor& Corridor, FColor Color = FColor::Cyan, float Duration = 10.f);
	static void DrawGraph(const UWorld* World, const FDungeonGraph& Graph, float Duration = 10.f);

	static constexpr float CellSize = 100.f;
};
