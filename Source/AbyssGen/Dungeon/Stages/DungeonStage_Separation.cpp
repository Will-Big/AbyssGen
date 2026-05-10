#include "DungeonStage_Separation.h"

#include "Core/DungeonContext.h"
#include "Core/DungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UDungeonStage_Separation::UDungeonStage_Separation()
{
	DisplayName = TEXT("2. Separation");
	Summary = TEXT("겹친 방들을 서로 밀어내 겹침을 제거한다. 끝나면 큰 방을 메인 방으로 선정한다.");
}

namespace
{
	/** 두 방이 (Padding 포함) 겹치면 분리 벡터 반환. */
	bool ComputePushVector(const FDungeonRoom& A, const FDungeonRoom& B, float Padding, FVector2D& OutPush)
	{
		const FVector2D Delta = B.Center - A.Center;
		const float MinX = (A.Size.X + B.Size.X) * 0.5f + Padding;
		const float MinY = (A.Size.Y + B.Size.Y) * 0.5f + Padding;
		const float OverlapX = MinX - FMath::Abs(Delta.X);
		const float OverlapY = MinY - FMath::Abs(Delta.Y);
		if (OverlapX <= 0.f || OverlapY <= 0.f) return false;

		// 더 작은 축으로만 분리 (덜 흔들림).
		if (OverlapX < OverlapY)
		{
			const float Sign = (Delta.X >= 0.f) ? 1.f : -1.f;
			OutPush = FVector2D(OverlapX * Sign * 0.5f, 0.f);
		}
		else
		{
			const float Sign = (Delta.Y >= 0.f) ? 1.f : -1.f;
			OutPush = FVector2D(0.f, OverlapY * Sign * 0.5f);
		}
		return true;
	}
}

void UDungeonStage_Separation::Execute(FDungeonContext& Ctx)
{
	const int32 N = Ctx.Rooms.Num();

	// --- 겹침 분리 ---
	for (int32 Iter = 0; Iter < MaxIterations; ++Iter)
	{
		bool bAnyOverlap = false;
		for (int32 i = 0; i < N; ++i)
		{
			for (int32 j = i + 1; j < N; ++j)
			{
				FVector2D Push;
				if (ComputePushVector(Ctx.Rooms[i], Ctx.Rooms[j], Padding, Push))
				{
					Ctx.Rooms[i].Center -= Push;
					Ctx.Rooms[j].Center += Push;
					bAnyOverlap = true;
				}
			}
		}
		if (!bAnyOverlap) break;
	}

	// --- 메인 방 선정 ---
	if (N == 0) return;
	float SumArea = 0.f;
	for (const FDungeonRoom& R : Ctx.Rooms)
	{
		SumArea += R.Size.X * R.Size.Y;
	}
	const float AvgArea = SumArea / N;
	const float Threshold = AvgArea * MainRoomSizeMultiplier;

	Ctx.MainRoomIndices.Reset();
	for (int32 i = 0; i < N; ++i)
	{
		const float Area = Ctx.Rooms[i].Size.X * Ctx.Rooms[i].Size.Y;
		Ctx.Rooms[i].bIsMain = (Area >= Threshold);
		if (Ctx.Rooms[i].bIsMain)
		{
			Ctx.MainRoomIndices.Add(i);
		}
	}
}

void UDungeonStage_Separation::DrawDebug(const FDungeonContext& Ctx) const
{
	UWorld* World = Ctx.World;
	if (!World) return;

	const float Cell = AbyssGen::Dungeon::CellSizeUU;
	for (const FDungeonRoom& Room : Ctx.Rooms)
	{
		const FVector Center(Room.Center.X * Cell, Room.Center.Y * Cell, 0.f);
		const FVector Extent(Room.Size.X * Cell * 0.5f, Room.Size.Y * Cell * 0.5f, Cell * 0.5f);
		const FColor Color = Room.bIsMain ? FColor::Yellow : FColor(120, 120, 120);
		const float Thickness = Room.bIsMain ? 3.f : 1.f;
		DrawDebugBox(World, Center + FVector(0, 0, Extent.Z), Extent, Color,
			true, -1.f, 0, Thickness);
	}
}
