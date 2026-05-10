#include "DungeonStage_ScatterRooms.h"

#include "Core/DungeonContext.h"
#include "Core/DungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UDungeonStage_ScatterRooms::UDungeonStage_ScatterRooms()
{
	DisplayName = TEXT("1. Scatter Rooms");
	Summary = TEXT("원형 영역 안에 방을 무작위로 흩뿌린다. 서로 겹쳐도 OK — 다음 단계가 분리한다.");
}

void UDungeonStage_ScatterRooms::Execute(FDungeonContext& Ctx)
{
	Ctx.Rooms.Reset();
	Ctx.Rooms.Reserve(RoomCount);

	const int32 Lo = FMath::Min(MinSize, MaxSize);
	const int32 Hi = FMath::Max(MinSize, MaxSize);

	for (int32 i = 0; i < RoomCount; ++i)
	{
		// 원반 안 균등분포: r = R*sqrt(u), θ = 2π*v
		const float U = Ctx.Rng.FRand();
		const float V = Ctx.Rng.FRand();
		const float R = SpawnRadius * FMath::Sqrt(U);
		const float Theta = 2.f * PI * V;

		FDungeonRoom Room;
		Room.Center = FVector2D(R * FMath::Cos(Theta), R * FMath::Sin(Theta));
		Room.Size = FIntPoint(
			Ctx.Rng.RandRange(Lo, Hi),
			Ctx.Rng.RandRange(Lo, Hi));
		Ctx.Rooms.Add(Room);
	}
}

void UDungeonStage_ScatterRooms::DrawDebug(const FDungeonContext& Ctx) const
{
	UWorld* World = Ctx.World;
	if (!World) return;

	const float Cell = AbyssGen::Dungeon::CellSizeUU;
	for (const FDungeonRoom& Room : Ctx.Rooms)
	{
		const FVector Center(Room.Center.X * Cell, Room.Center.Y * Cell, 0.f);
		const FVector Extent(Room.Size.X * Cell * 0.5f, Room.Size.Y * Cell * 0.5f, Cell * 0.5f);
		DrawDebugBox(World, Center + FVector(0, 0, Extent.Z), Extent, FColor(150, 150, 150),
			true, -1.f, 0, 1.f);
	}
}
