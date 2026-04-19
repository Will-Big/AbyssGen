#include "DungeonDebugDraw.h"
#include "DungeonConstants.h"
#include "DrawDebugHelpers.h"

using DungeonConstants::CellSize;

static FVector RoomWorldCenter(const FRoom& Room)
{
	return FVector(
		Room.GridCenter.X * CellSize,
		Room.GridCenter.Y * CellSize,
		0.f
	);
}

void FDungeonDebugDraw::DrawRoom(const UWorld* World, const FRoom& Room, FColor Color, float Duration)
{
	if (!World) return;

	const FVector Center = RoomWorldCenter(Room);
	const FVector Extent = FVector(
		Room.GridSize.X * CellSize * 0.5f,
		Room.GridSize.Y * CellSize * 0.5f,
		50.f);

	DrawDebugBox(World, Center, Extent, Color, false, Duration, 0, 8.f);
}

void FDungeonDebugDraw::DrawEdge(const UWorld* World, const FDungeonEdge& Edge, const TArray<FRoom>& Rooms, FColor Color, float Duration)
{
	if (!World) return;
	if (!Rooms.IsValidIndex(Edge.RoomA) || !Rooms.IsValidIndex(Edge.RoomB)) return;

	const FVector From = RoomWorldCenter(Rooms[Edge.RoomA]);
	const FVector To   = RoomWorldCenter(Rooms[Edge.RoomB]);

	DrawDebugLine(World, From, To, Color, false, Duration, 0, 12.f);
}

void FDungeonDebugDraw::DrawCorridor(const UWorld* World, const FLCorridor& Corridor, FColor Color, float Duration)
{
	if (!World) return;

	const FVector Start  = FVector(Corridor.Start.X  * CellSize, Corridor.Start.Y  * CellSize, 0.f);
	const FVector Corner = FVector(Corridor.Corner.X * CellSize, Corridor.Corner.Y * CellSize, 0.f);
	const FVector End    = FVector(Corridor.End.X    * CellSize, Corridor.End.Y    * CellSize, 0.f);

	DrawDebugLine(World, Start,  Corner, Color, false, Duration, 0, 12.f);
	DrawDebugLine(World, Corner, End,    Color, false, Duration, 0, 12.f);
}

void FDungeonDebugDraw::DrawGraph(const UWorld* World, const FDungeonGraph& Graph, float Duration)
{
	for (const FRoom& Room : Graph.Rooms)
	{
		FColor Color = FColor::White;
		if (Room.bIsMain)
		{
			switch (Room.Type)
			{
			case ERoomType::Start:    Color = FColor::Green;  break;
			case ERoomType::Boss:     Color = FColor::Red;    break;
			case ERoomType::Treasure: Color = FColor::Yellow; break;
			default:                  Color = FColor(100, 200, 100); break;
			}
		}
		DrawRoom(World, Room, Color, Duration);

		for (const FIntPoint& Door : Room.DoorCells)
		{
			const FVector DoorCenter(Door.X * CellSize, Door.Y * CellSize, 0.f);
			DrawDebugBox(World, DoorCenter, FVector(CellSize * 0.4f, CellSize * 0.4f, 60.f),
				FColor::Blue, false, Duration, 0, 4.f);
		}
	}

	for (const FDungeonEdge& Edge : Graph.DelaunayEdges)
	{
		DrawEdge(World, Edge, Graph.Rooms, FColor(128, 128, 128), Duration);
	}

	for (const FDungeonEdge& Edge : Graph.MstEdges)
	{
		DrawEdge(World, Edge, Graph.Rooms, FColor::Yellow, Duration);
	}

	for (const FDungeonEdge& Edge : Graph.LoopEdges)
	{
		DrawEdge(World, Edge, Graph.Rooms, FColor::Orange, Duration);
	}

	for (const FLCorridor& Corridor : Graph.Corridors)
	{
		DrawCorridor(World, Corridor, FColor::Cyan, Duration);
	}
}
