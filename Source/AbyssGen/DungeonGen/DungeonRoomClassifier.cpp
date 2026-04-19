#include "DungeonRoomClassifier.h"
#include "DungeonLog.h"

void DungeonRoomClassifier::AssignSpecialRooms(FDungeonGraph& Graph, FRandomStream& Rand)
{
	for (int32 Idx : Graph.MainRoomIndices)
		Graph.Rooms[Idx].Type = ERoomType::Normal;

	if (Graph.MainRoomIndices.IsEmpty()) return;

	// Start: 원점에 가장 가까운 메인 방
	int32 StartIdx = Graph.MainRoomIndices[0];
	float MinDist  = Graph.Rooms[StartIdx].FloatCenter.SizeSquared();
	for (int32 Idx : Graph.MainRoomIndices)
	{
		const float D = Graph.Rooms[Idx].FloatCenter.SizeSquared();
		if (D < MinDist) { MinDist = D; StartIdx = Idx; }
	}
	Graph.Rooms[StartIdx].Type = ERoomType::Start;

	// Boss: Start에서 가장 먼 메인 방
	int32 BossIdx = StartIdx;
	float MaxDist = 0.f;
	const FVector2D StartCenter = Graph.Rooms[StartIdx].FloatCenter;
	for (int32 Idx : Graph.MainRoomIndices)
	{
		if (Idx == StartIdx) continue;
		const float D = FVector2D::DistSquared(Graph.Rooms[Idx].FloatCenter, StartCenter);
		if (D > MaxDist) { MaxDist = D; BossIdx = Idx; }
	}
	Graph.Rooms[BossIdx].Type = ERoomType::Boss;

	// MST degree 계산
	TMap<int32, int32> Degree;
	for (int32 Idx : Graph.MainRoomIndices) Degree.Add(Idx, 0);
	for (const FDungeonEdge& E : Graph.MstEdges)
	{
		++Degree[E.RoomA];
		++Degree[E.RoomB];
	}

	// Treasure: Start·Boss 제외 MST leaf 중 랜덤
	TArray<int32> Leaves;
	for (int32 Idx : Graph.MainRoomIndices)
	{
		if (Idx == StartIdx || Idx == BossIdx) continue;
		if (Degree.FindRef(Idx) == 1) Leaves.Add(Idx);
	}
	if (!Leaves.IsEmpty())
		Graph.Rooms[Leaves[Rand.RandRange(0, Leaves.Num() - 1)]].Type = ERoomType::Treasure;

	UE_LOG(LogDungeonGen, Log, TEXT("Special rooms — Start:%d Boss:%d Leaves:%d"),
		StartIdx, BossIdx, Leaves.Num());
}
