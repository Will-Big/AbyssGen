#include "DungeonStage_LCorridors.h"

#include "Core/DungeonContext.h"
#include "Core/DungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UDungeonStage_LCorridors::UDungeonStage_LCorridors()
{
	DisplayName = TEXT("5. L Corridors");
	Summary = TEXT("활성 간선 각각에 대해 시작-꺾임-끝 3점의 L자 복도 경로를 만든다.");
}

namespace
{
	FIntPoint ToCell(const FVector2D& V)
	{
		return FIntPoint(FMath::RoundToInt(V.X), FMath::RoundToInt(V.Y));
	}

	FLCorridor MakeCorridor(const FDungeonEdge& E, const TArray<FDungeonRoom>& Rooms, FRandomStream& Rng)
	{
		const FIntPoint A = ToCell(Rooms[E.RoomA].Center);
		const FIntPoint B = ToCell(Rooms[E.RoomB].Center);

		FLCorridor C;
		C.RoomA = E.RoomA;
		C.RoomB = E.RoomB;
		C.Start = A;
		C.End = B;
		// 50% 확률로 가로→세로 vs 세로→가로
		C.Corner = (Rng.FRand() < 0.5f) ? FIntPoint(B.X, A.Y) : FIntPoint(A.X, B.Y);
		return C;
	}
}

void UDungeonStage_LCorridors::Execute(FDungeonContext& Ctx)
{
	Ctx.Corridors.Reset();
	Ctx.Corridors.Reserve(Ctx.MstEdges.Num() + Ctx.LoopEdges.Num());

	for (const FDungeonEdge& E : Ctx.MstEdges)
	{
		Ctx.Corridors.Add(MakeCorridor(E, Ctx.Rooms, Ctx.Rng));
	}
	for (const FDungeonEdge& E : Ctx.LoopEdges)
	{
		Ctx.Corridors.Add(MakeCorridor(E, Ctx.Rooms, Ctx.Rng));
	}
}

void UDungeonStage_LCorridors::DrawDebug(const FDungeonContext& Ctx) const
{
	UWorld* World = Ctx.World;
	if (!World) return;

	const float Cell = AbyssGen::Dungeon::CellSizeUU;
	const float Z = Cell * 0.6f;

	auto ToWorld = [Cell, Z](const FIntPoint& P)
	{
		return FVector(P.X * Cell, P.Y * Cell, Z);
	};

	for (const FLCorridor& C : Ctx.Corridors)
	{
		DrawDebugLine(World, ToWorld(C.Start), ToWorld(C.Corner),
			FColor(255, 140, 0), true, -1.f, 0, 5.f);
		DrawDebugLine(World, ToWorld(C.Corner), ToWorld(C.End),
			FColor(255, 140, 0), true, -1.f, 0, 5.f);
	}
}
