#include "DungeonDebugDraw.h"
#include "DrawDebugHelpers.h"

static FVector RoomWorldCenter(const FRoom& Room)
{
	return FVector(
		Room.GridCenter.X * FDungeonDebugDraw::CellSize,
		Room.GridCenter.Y * FDungeonDebugDraw::CellSize,
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
		50.f
	);

	DrawDebugBox(World, Center, Extent, Color, false, Duration, 0, 8.f);
}

void FDungeonDebugDraw::DrawEdge(const UWorld* World, const FDungeonEdge& Edge, const TArray<FRoom>& Rooms, FColor Color, float Duration)
{
	if (!World) return;
	if (!Rooms.IsValidIndex(Edge.RoomA) || !Rooms.IsValidIndex(Edge.RoomB)) return;

	const FVector From = RoomWorldCenter(Rooms[Edge.RoomA]);
	const FVector To   = RoomWorldCenter(Rooms[Edge.RoomB]);

	DrawDebugLine(World, From, To, Color, false, Duration, 0, 4.f);
}

void FDungeonDebugDraw::DrawGraph(const UWorld* World, const FDungeonGraph& Graph, float Duration)
{
	for (const FRoom& Room : Graph.Rooms)
	{
		const FColor Color = Room.bIsMain ? FColor::Green : FColor::White;
		DrawRoom(World, Room, Color, Duration);
	}

	for (const FDungeonEdge& Edge : Graph.MstEdges)
	{
		DrawEdge(World, Edge, Graph.Rooms, FColor::Yellow, Duration);
	}

	for (const FDungeonEdge& Edge : Graph.LoopEdges)
	{
		DrawEdge(World, Edge, Graph.Rooms, FColor::Orange, Duration);
	}
}
