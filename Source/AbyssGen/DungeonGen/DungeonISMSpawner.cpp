#include "DungeonISMSpawner.h"
#include "DungeonConstants.h"
#include "Components/InstancedStaticMeshComponent.h"

using DungeonConstants::CellSize;

static constexpr int32 WallLayers = 4; // 4 * 100cm = 400cm total, ~300cm visible above floor

// Walks every grid cell in a corridor path (Start→Corner→End), no duplicate at corner
static void ForEachCorridorCell(const FLCorridor& C, TFunctionRef<void(FIntPoint)> Fn)
{
	auto Walk = [&](FIntPoint From, FIntPoint To)
	{
		const int32 DX = FMath::Sign(To.X - From.X);
		const int32 DY = FMath::Sign(To.Y - From.Y);
		FIntPoint P = From;
		while (true) { Fn(P); if (P == To) break; P.X += DX; P.Y += DY; }
	};

	Walk(C.Start, C.Corner);
	if (C.Corner != C.End)
	{
		const int32 DX = FMath::Sign(C.End.X - C.Corner.X);
		const int32 DY = FMath::Sign(C.End.Y - C.Corner.Y);
		Walk(FIntPoint(C.Corner.X + DX, C.Corner.Y + DY), C.End);
	}
}

// Builds a set of all cells occupied by main rooms
static TSet<FIntPoint> BuildRoomCellSet(const FDungeonGraph& Graph)
{
	TSet<FIntPoint> Cells;
	for (int32 Idx : Graph.MainRoomIndices)
	{
		const FRoom& Room = Graph.Rooms[Idx];
		const int32 OX = Room.GridCenter.X - Room.GridSize.X / 2;
		const int32 OY = Room.GridCenter.Y - Room.GridSize.Y / 2;
		for (int32 Cx = OX; Cx < OX + Room.GridSize.X; ++Cx)
			for (int32 Cy = OY; Cy < OY + Room.GridSize.Y; ++Cy)
				Cells.Add(FIntPoint(Cx, Cy));
	}
	return Cells;
}

// Builds a set of all cells occupied by corridors
static TSet<FIntPoint> BuildCorridorCellSet(const FDungeonGraph& Graph)
{
	TSet<FIntPoint> Cells;
	for (const FLCorridor& Corridor : Graph.Corridors)
		ForEachCorridorCell(Corridor, [&](FIntPoint P) { Cells.Add(P); });
	return Cells;
}

void DungeonISMSpawner::SpawnFloorTiles(UInstancedStaticMeshComponent* ISM, const FDungeonGraph& Graph)
{
	if (!ISM) return;

	ISM->ClearInstances();

	TArray<FTransform> Transforms;
	Transforms.Reserve(4096);

	const FVector TileScale(CellSize / 100.f, CellSize / 100.f, 1.f);

	// Room floors
	for (int32 Idx : Graph.MainRoomIndices)
	{
		const FRoom& Room = Graph.Rooms[Idx];
		const int32 OX = Room.GridCenter.X - Room.GridSize.X / 2;
		const int32 OY = Room.GridCenter.Y - Room.GridSize.Y / 2;

		for (int32 Cx = OX; Cx < OX + Room.GridSize.X; ++Cx)
			for (int32 Cy = OY; Cy < OY + Room.GridSize.Y; ++Cy)
				Transforms.Emplace(FQuat::Identity,
					FVector(Cx * CellSize, Cy * CellSize, 0.f), TileScale);
	}

	// Corridor floors (skip cells already covered by rooms)
	const TSet<FIntPoint> RoomCells = BuildRoomCellSet(Graph);
	TSet<FIntPoint> Placed;

	for (const FLCorridor& Corridor : Graph.Corridors)
	{
		ForEachCorridorCell(Corridor, [&](FIntPoint P)
		{
			if (RoomCells.Contains(P) || Placed.Contains(P)) return;
			Placed.Add(P);
			Transforms.Emplace(FQuat::Identity,
				FVector(P.X * CellSize, P.Y * CellSize, 0.f), TileScale);
		});
	}

	ISM->AddInstances(Transforms, /*bShouldReturnIndices=*/false, /*bWorldSpace=*/true);
}

void DungeonISMSpawner::SpawnWallTiles(UInstancedStaticMeshComponent* ISM, const FDungeonGraph& Graph)
{
	if (!ISM) return;

	ISM->ClearInstances();

	TArray<FTransform> Transforms;
	Transforms.Reserve(8192);

	const FVector TileScale(CellSize / 100.f, CellSize / 100.f, 1.f);

	auto AddWallStack = [&](int32 Cx, int32 Cy)
	{
		for (int32 Layer = 0; Layer < WallLayers; ++Layer)
			Transforms.Emplace(FQuat::Identity,
				FVector(Cx * CellSize, Cy * CellSize, Layer * 100.f), TileScale);
	};

	// Room walls
	for (int32 Idx : Graph.MainRoomIndices)
	{
		const FRoom& Room = Graph.Rooms[Idx];
		const int32 OX = Room.GridCenter.X - Room.GridSize.X / 2;
		const int32 OY = Room.GridCenter.Y - Room.GridSize.Y / 2;
		TSet<FIntPoint> DoorSet(Room.DoorCells);

		for (int32 Cx = OX; Cx < OX + Room.GridSize.X; ++Cx)
		{
			for (int32 Cy = OY; Cy < OY + Room.GridSize.Y; ++Cy)
			{
				const bool bBorder =
					Cx == OX || Cx == OX + Room.GridSize.X - 1 ||
					Cy == OY || Cy == OY + Room.GridSize.Y - 1;
				if (!bBorder || DoorSet.Contains(FIntPoint(Cx, Cy))) continue;
				AddWallStack(Cx, Cy);
			}
		}
	}

	// Corridor walls (sides perpendicular to corridor direction)
	const TSet<FIntPoint> RoomCells     = BuildRoomCellSet(Graph);
	const TSet<FIntPoint> CorridorCells = BuildCorridorCellSet(Graph);
	TSet<FIntPoint> PlacedWalls;

	auto TryAddWall = [&](int32 Cx, int32 Cy)
	{
		const FIntPoint P(Cx, Cy);
		if (RoomCells.Contains(P) || CorridorCells.Contains(P) || PlacedWalls.Contains(P)) return;
		PlacedWalls.Add(P);
		AddWallStack(Cx, Cy);
	};

	for (const FLCorridor& Corridor : Graph.Corridors)
	{
		auto PlaceSegmentWalls = [&](FIntPoint From, FIntPoint To)
		{
			if (From == To) return;
			const int32 DX = FMath::Sign(To.X - From.X);
			const int32 DY = FMath::Sign(To.Y - From.Y);
			FIntPoint P = From;
			while (true)
			{
				if (DX != 0) { TryAddWall(P.X, P.Y + 1); TryAddWall(P.X, P.Y - 1); }
				else          { TryAddWall(P.X + 1, P.Y); TryAddWall(P.X - 1, P.Y); }
				if (P == To) break;
				P.X += DX; P.Y += DY;
			}
		};

		PlaceSegmentWalls(Corridor.Start, Corridor.Corner);
		PlaceSegmentWalls(Corridor.Corner, Corridor.End);
	}

	ISM->AddInstances(Transforms, /*bShouldReturnIndices=*/false, /*bWorldSpace=*/true);
}
