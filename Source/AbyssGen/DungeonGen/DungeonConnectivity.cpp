#include "DungeonConnectivity.h"
#include "DungeonLog.h"

void DungeonConnectivity::Verify(const FDungeonGraph& Graph)
{
	if (Graph.MainRoomIndices.IsEmpty()) return;

	TMap<int32, TArray<int32>> Adj;
	for (int32 Idx : Graph.MainRoomIndices) Adj.Add(Idx, {});

	auto AddEdge = [&](const FDungeonEdge& E)
	{
		if (Adj.Contains(E.RoomA)) Adj[E.RoomA].Add(E.RoomB);
		if (Adj.Contains(E.RoomB)) Adj[E.RoomB].Add(E.RoomA);
	};
	for (const FDungeonEdge& E : Graph.MstEdges)  AddEdge(E);
	for (const FDungeonEdge& E : Graph.LoopEdges) AddEdge(E);

	int32 StartIdx = INDEX_NONE;
	for (int32 Idx : Graph.MainRoomIndices)
	{
		if (Graph.Rooms[Idx].Type == ERoomType::Start) { StartIdx = Idx; break; }
	}
	if (StartIdx == INDEX_NONE) StartIdx = Graph.MainRoomIndices[0];

	TSet<int32> Visited;
	TQueue<int32> Queue;
	Queue.Enqueue(StartIdx);
	Visited.Add(StartIdx);

	while (!Queue.IsEmpty())
	{
		int32 Cur;
		Queue.Dequeue(Cur);
		for (int32 Next : Adj[Cur])
		{
			if (!Visited.Contains(Next))
			{
				Visited.Add(Next);
				Queue.Enqueue(Next);
			}
		}
	}

	const int32 Total   = Graph.MainRoomIndices.Num();
	const int32 Reached = Visited.Num();

	if (Reached == Total)
	{
		UE_LOG(LogDungeonGen, Log, TEXT("Connectivity OK — all %d main rooms reachable"), Total);
	}
	else
	{
		UE_LOG(LogDungeonGen, Warning, TEXT("Connectivity FAIL — reached %d / %d main rooms"), Reached, Total);
	}
}
