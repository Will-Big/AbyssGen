#include "DungeonGraphBuilder.h"
#include "DungeonTriangulate.h"
#include "DungeonLog.h"

void DungeonGraphBuilder::SelectMainRooms(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config)
{
	Graph.MainRoomIndices.Reset();

	const int32 N = Graph.Rooms.Num();
	if (N == 0) return;

	float SumArea = 0.f;
	for (const FRoom& R : Graph.Rooms)
		SumArea += static_cast<float>(R.GridSize.X * R.GridSize.Y);
	const float Threshold = (SumArea / N) * Config.MainRoomSizeMultiplier;

	for (int32 i = 0; i < N; ++i)
	{
		FRoom& R = Graph.Rooms[i];
		R.bIsMain = (static_cast<float>(R.GridSize.X * R.GridSize.Y) >= Threshold);
		if (R.bIsMain)
			Graph.MainRoomIndices.Add(i);
	}
}

void DungeonGraphBuilder::Triangulate(FDungeonGraph& Graph)
{
	Graph.DelaunayEdges.Reset();

	const int32 MainCount = Graph.MainRoomIndices.Num();
	if (MainCount < 3) return;

	TArray<FVector2D> Points;
	Points.Reserve(MainCount);
	for (int32 Idx : Graph.MainRoomIndices)
		Points.Add(Graph.Rooms[Idx].FloatCenter);

	TArray<int32> Triangles;
	DungeonTriangulate::Triangulate(Points, Triangles);

	for (int32 t = 0; t + 2 < Triangles.Num(); t += 3)
	{
		const int32 I0 = Graph.MainRoomIndices[Triangles[t + 0]];
		const int32 I1 = Graph.MainRoomIndices[Triangles[t + 1]];
		const int32 I2 = Graph.MainRoomIndices[Triangles[t + 2]];

		Graph.DelaunayEdges.Add({ I0, I1, 0.f });
		Graph.DelaunayEdges.Add({ I1, I2, 0.f });
		Graph.DelaunayEdges.Add({ I2, I0, 0.f });
	}
}

void DungeonGraphBuilder::DeduplicateEdges(FDungeonGraph& Graph)
{
	TSet<TPair<int32, int32>> Seen;
	TArray<FDungeonEdge> Unique;
	Unique.Reserve(Graph.DelaunayEdges.Num());

	for (FDungeonEdge& Edge : Graph.DelaunayEdges)
	{
		const int32 Lo = FMath::Min(Edge.RoomA, Edge.RoomB);
		const int32 Hi = FMath::Max(Edge.RoomA, Edge.RoomB);
		const TPair<int32, int32> Key(Lo, Hi);

		if (Seen.Contains(Key)) continue;
		Seen.Add(Key);

		Edge.RoomA  = Lo;
		Edge.RoomB  = Hi;
		Edge.Weight = FVector2D::Distance(Graph.Rooms[Lo].FloatCenter, Graph.Rooms[Hi].FloatCenter);
		Unique.Add(Edge);
	}

	Graph.DelaunayEdges = MoveTemp(Unique);
	UE_LOG(LogDungeonGen, Log, TEXT("Delaunay edges after dedup: %d"), Graph.DelaunayEdges.Num());
}

void DungeonGraphBuilder::BuildMST(FDungeonGraph& Graph)
{
	Graph.MstEdges.Reset();

	const int32 N = Graph.Rooms.Num();
	if (Graph.DelaunayEdges.Num() == 0) return;

	TArray<int32> Parent, Rank;
	Parent.SetNumUninitialized(N);
	Rank.SetNumZeroed(N);
	for (int32 i = 0; i < N; ++i) Parent[i] = i;

	TFunction<int32(int32)> Find = [&](int32 X) -> int32
	{
		if (Parent[X] != X) Parent[X] = Find(Parent[X]);
		return Parent[X];
	};

	auto Union = [&](int32 X, int32 Y) -> bool
	{
		const int32 RX = Find(X), RY = Find(Y);
		if (RX == RY) return false;
		if      (Rank[RX] < Rank[RY]) Parent[RX] = RY;
		else if (Rank[RX] > Rank[RY]) Parent[RY] = RX;
		else { Parent[RY] = RX; ++Rank[RX]; }
		return true;
	};

	TArray<FDungeonEdge> Sorted = Graph.DelaunayEdges;
	Sorted.Sort([](const FDungeonEdge& A, const FDungeonEdge& B){ return A.Weight < B.Weight; });

	for (const FDungeonEdge& Edge : Sorted)
	{
		if (Union(Edge.RoomA, Edge.RoomB))
			Graph.MstEdges.Add(Edge);
	}

	UE_LOG(LogDungeonGen, Log, TEXT("MST edges: %d"), Graph.MstEdges.Num());
}

void DungeonGraphBuilder::ReviveLoops(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config, FRandomStream& Rand)
{
	Graph.LoopEdges.Reset();

	TSet<TPair<int32, int32>> MstSet;
	MstSet.Reserve(Graph.MstEdges.Num());
	for (const FDungeonEdge& E : Graph.MstEdges)
		MstSet.Add(TPair<int32, int32>(E.RoomA, E.RoomB));

	TArray<FDungeonEdge> Candidates;
	for (const FDungeonEdge& E : Graph.DelaunayEdges)
	{
		if (!MstSet.Contains(TPair<int32, int32>(E.RoomA, E.RoomB)))
			Candidates.Add(E);
	}

	const int32 PickCount = FMath::RoundToInt(Candidates.Num() * FMath::Clamp(Config.LoopRatio, 0.f, 1.f));
	for (int32 i = 0; i < PickCount; ++i)
	{
		const int32 j = Rand.RandRange(i, Candidates.Num() - 1);
		Candidates.Swap(i, j);
		Graph.LoopEdges.Add(Candidates[i]);
	}

	UE_LOG(LogDungeonGen, Log, TEXT("Loop edges: %d / %d candidates (LoopRatio=%.2f)"),
		Graph.LoopEdges.Num(), Candidates.Num(), Config.LoopRatio);
}
