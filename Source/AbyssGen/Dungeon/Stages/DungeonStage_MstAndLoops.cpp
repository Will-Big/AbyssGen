#include "DungeonStage_MstAndLoops.h"

#include "Core/DungeonContext.h"
#include "Core/DungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UDungeonStage_MstAndLoops::UDungeonStage_MstAndLoops()
{
	DisplayName = TEXT("4. MST + Loops");
	Summary = TEXT("후보 간선에서 최소 신장 트리를 뽑고, 남은 간선 일부를 루프로 되살린다. LoopRatio로 조절.");
}

namespace
{
	struct FUnionFind
	{
		TArray<int32> Parent;
		void Init(int32 N)
		{
			Parent.SetNum(N);
			for (int32 i = 0; i < N; ++i) Parent[i] = i;
		}
		int32 Find(int32 X)
		{
			while (Parent[X] != X)
			{
				Parent[X] = Parent[Parent[X]]; // path compression
				X = Parent[X];
			}
			return X;
		}
		bool Union(int32 A, int32 B)
		{
			const int32 RA = Find(A), RB = Find(B);
			if (RA == RB) return false;
			Parent[RA] = RB;
			return true;
		}
	};
}

void UDungeonStage_MstAndLoops::Execute(FDungeonContext& Ctx)
{
	Ctx.MstEdges.Reset();
	Ctx.LoopEdges.Reset();

	if (Ctx.DelaunayEdges.Num() == 0) return;

	// 길이 오름차순 정렬한 사본.
	TArray<FDungeonEdge> Sorted = Ctx.DelaunayEdges;
	Sorted.Sort([](const FDungeonEdge& A, const FDungeonEdge& B) { return A.Length < B.Length; });

	// 모든 방 인덱스(메인이 아닌 것 포함)를 다 받아도 동작하도록 Rooms.Num() 크기로 UF 잡음.
	FUnionFind UF;
	UF.Init(Ctx.Rooms.Num());

	TArray<FDungeonEdge> Leftover;
	for (const FDungeonEdge& E : Sorted)
	{
		if (UF.Union(E.RoomA, E.RoomB))
		{
			Ctx.MstEdges.Add(E);
		}
		else
		{
			Leftover.Add(E);
		}
	}

	// 루프 복원: 남은 간선 중 LoopRatio 비율만큼 섞어 추가.
	const int32 LoopCount = FMath::RoundToInt(Leftover.Num() * FMath::Clamp(LoopRatio, 0.f, 1.f));
	for (int32 i = 0; i < Leftover.Num(); ++i)
	{
		const int32 Swap = Ctx.Rng.RandRange(i, Leftover.Num() - 1);
		Leftover.Swap(i, Swap);
	}
	for (int32 i = 0; i < LoopCount; ++i)
	{
		Ctx.LoopEdges.Add(Leftover[i]);
	}
}

void UDungeonStage_MstAndLoops::DrawDebug(const FDungeonContext& Ctx) const
{
	UWorld* World = Ctx.World;
	if (!World) return;

	const float Cell = AbyssGen::Dungeon::CellSizeUU;
	const float Z = Cell * 1.2f;

	auto DrawEdges = [&](const TArray<FDungeonEdge>& Edges, FColor Color, float Thickness)
	{
		for (const FDungeonEdge& E : Edges)
		{
			const FVector2D& A = Ctx.Rooms[E.RoomA].Center;
			const FVector2D& B = Ctx.Rooms[E.RoomB].Center;
			DrawDebugLine(World,
				FVector(A.X * Cell, A.Y * Cell, Z),
				FVector(B.X * Cell, B.Y * Cell, Z),
				Color, true, -1.f, 0, Thickness);
		}
	};

	DrawEdges(Ctx.MstEdges, FColor::White, 4.f);
	DrawEdges(Ctx.LoopEdges, FColor::Cyan, 3.f);
}
