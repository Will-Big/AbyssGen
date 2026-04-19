#include "DungeonCorridorGen.h"
#include "DungeonLog.h"

void DungeonCorridorGen::ComputeCorridors(FDungeonGraph& Graph, FRandomStream& Rand)
{
	Graph.Corridors.Reset();

	TArray<FDungeonEdge> AllEdges = Graph.MstEdges;
	AllEdges.Append(Graph.LoopEdges);

	for (const FDungeonEdge& Edge : AllEdges)
	{
		const FIntPoint A = Graph.Rooms[Edge.RoomA].GridCenter;
		const FIntPoint B = Graph.Rooms[Edge.RoomB].GridCenter;

		FLCorridor Corridor;
		Corridor.RoomA  = Edge.RoomA;
		Corridor.RoomB  = Edge.RoomB;
		Corridor.Start  = A;
		Corridor.End    = B;
		Corridor.Corner = Rand.RandRange(0, 1) == 0
			? FIntPoint(B.X, A.Y)
			: FIntPoint(A.X, B.Y);

		Graph.Corridors.Add(Corridor);
	}

	UE_LOG(LogDungeonGen, Log, TEXT("Corridors: %d"), Graph.Corridors.Num());
}

void DungeonCorridorGen::MarkDoorCells(FDungeonGraph& Graph)
{
	for (FRoom& R : Graph.Rooms) R.DoorCells.Reset();

	auto IsInsideRoom = [](const FIntPoint& Cell, const FRoom& Room) -> bool
	{
		const int32 OX = Room.GridCenter.X - Room.GridSize.X / 2;
		const int32 OY = Room.GridCenter.Y - Room.GridSize.Y / 2;
		return Cell.X >= OX && Cell.X < OX + Room.GridSize.X
		    && Cell.Y >= OY && Cell.Y < OY + Room.GridSize.Y;
	};

	for (const FLCorridor& C : Graph.Corridors)
	{
		// 복도 전체 경로를 순서대로 수집 (Start → Corner → End, Corner 중복 없음)
		TArray<FIntPoint> Path;

		auto Walk = [&](FIntPoint From, FIntPoint To)
		{
			const int32 DX = FMath::Sign(To.X - From.X);
			const int32 DY = FMath::Sign(To.Y - From.Y);
			FIntPoint P = From;
			while (true) { Path.Add(P); if (P == To) break; P.X += DX; P.Y += DY; }
		};

		Walk(C.Start, C.Corner);
		if (C.Corner != C.End)
		{
			const int32 DX = FMath::Sign(C.End.X - C.Corner.X);
			const int32 DY = FMath::Sign(C.End.Y - C.Corner.Y);
			Walk(FIntPoint(C.Corner.X + DX, C.Corner.Y + DY), C.End);
		}

		// 인접 셀 쌍을 순회하며 방 경계 진입/탈출 감지
		for (int32 i = 0; i + 1 < Path.Num(); ++i)
		{
			const FIntPoint& Prev = Path[i];
			const FIntPoint& Curr = Path[i + 1];

			for (int32 RoomIdx = 0; RoomIdx < Graph.Rooms.Num(); ++RoomIdx)
			{
				FRoom& R = Graph.Rooms[RoomIdx];
				const bool bPrev = IsInsideRoom(Prev, R);
				const bool bCurr = IsInsideRoom(Curr, R);

				// 탈출: Prev가 방 안의 마지막 셀 → 문
				if (bPrev && !bCurr && !R.DoorCells.Contains(Prev))
					R.DoorCells.Add(Prev);
				// 진입: Curr가 방 안의 첫 셀 → 문
				else if (!bPrev && bCurr && !R.DoorCells.Contains(Curr))
					R.DoorCells.Add(Curr);
			}
		}
	}

	int32 TotalDoors = 0;
	for (const FRoom& R : Graph.Rooms) TotalDoors += R.DoorCells.Num();
	UE_LOG(LogDungeonGen, Log, TEXT("Door cells marked: %d"), TotalDoors);
}
