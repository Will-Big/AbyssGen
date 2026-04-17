#include "DungeonGenerator.h"
#include "DungeonDebugDraw.h"
#include "DrawDebugHelpers.h"
#include "DungeonTriangulate.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADungeonGenerator::Generate()
{
	DungeonGraph.Reset();

	if (bRandomizeSeed)
	{
		Seed = FMath::Rand();
	}
	DungeonGraph.Seed = Seed;

	FRandomStream Rand(Seed);

	GenerateRooms(Rand);
	const int32 Iterations = RunSeparation();
	SelectMainRooms();
	Triangulate();
	DeduplicateEdges();
	BuildMST();
	ReviveLoops(Rand);

	if (UWorld* World = GetWorld())
	{
		FlushPersistentDebugLines(World);
	}
	FDungeonDebugDraw::DrawGraph(GetWorld(), DungeonGraph);

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Generate() Rooms=%d Main=%d Seed=%d SepIters=%d"),
		DungeonGraph.Rooms.Num(), DungeonGraph.MainRoomIndices.Num(), Seed, Iterations);
}

void ADungeonGenerator::StepSeparation()
{
	const int32 Overlapping = SeparationStep();

	if (UWorld* World = GetWorld())
	{
		FlushPersistentDebugLines(World);
	}
	FDungeonDebugDraw::DrawGraph(GetWorld(), DungeonGraph);

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] SeparationStep overlapping pairs=%d"), Overlapping);
}

void ADungeonGenerator::GenerateRooms(FRandomStream& Rand)
{
	DungeonGraph.Rooms.Reserve(TotalRoomCount);

	for (int32 i = 0; i < TotalRoomCount; ++i)
	{
		const float R     = SpawnRadius * FMath::Sqrt(Rand.GetFraction());
		const float Theta = Rand.GetFraction() * 2.f * PI;

		FRoom Room;
		Room.FloatCenter = FVector2D(R * FMath::Cos(Theta), R * FMath::Sin(Theta));
		Room.GridCenter  = FIntPoint(FMath::RoundToInt(Room.FloatCenter.X), FMath::RoundToInt(Room.FloatCenter.Y));
		Room.GridSize    = FIntPoint(Rand.RandRange(6, 12), Rand.RandRange(6, 12));

		DungeonGraph.Rooms.Add(Room);
	}
}

int32 ADungeonGenerator::SeparationStep()
{
	const int32 N = DungeonGraph.Rooms.Num();
	TArray<FVector2D> Pushes;
	Pushes.SetNumZeroed(N);

	int32 Overlapping = 0;

	for (int32 i = 0; i < N; ++i)
	{
		for (int32 j = i + 1; j < N; ++j)
		{
			const FRoom& A = DungeonGraph.Rooms[i];
			const FRoom& B = DungeonGraph.Rooms[j];

			const FVector2D Delta = A.FloatCenter - B.FloatCenter;
			const float MinSepX = (A.GridSize.X + B.GridSize.X) * 0.5f + SeparationPadding;
			const float MinSepY = (A.GridSize.Y + B.GridSize.Y) * 0.5f + SeparationPadding;

			if (FMath::Abs(Delta.X) >= MinSepX) continue;
			if (FMath::Abs(Delta.Y) >= MinSepY) continue;

			++Overlapping;

			// 중심이 동일하면 대칭 파괴용 기본 방향
			FVector2D Dir = Delta;
			if (Dir.IsNearlyZero())
			{
				Dir = FVector2D(1.f, 0.f);
			}
			else
			{
				Dir.Normalize();
			}

			Pushes[i] += Dir;
			Pushes[j] -= Dir;
		}
	}

	for (int32 i = 0; i < N; ++i)
	{
		FRoom& R = DungeonGraph.Rooms[i];
		R.FloatCenter += Pushes[i];
		R.GridCenter   = FIntPoint(FMath::RoundToInt(R.FloatCenter.X), FMath::RoundToInt(R.FloatCenter.Y));
	}

	return Overlapping;
}

void ADungeonGenerator::SelectMainRooms()
{
	DungeonGraph.MainRoomIndices.Reset();

	const int32 N = DungeonGraph.Rooms.Num();
	if (N == 0) return;

	float SumArea = 0.f;
	for (const FRoom& R : DungeonGraph.Rooms)
	{
		SumArea += static_cast<float>(R.GridSize.X * R.GridSize.Y);
	}
	const float Threshold = (SumArea / N) * MainRoomSizeMultiplier;

	for (int32 i = 0; i < N; ++i)
	{
		FRoom& R = DungeonGraph.Rooms[i];
		const float Area = static_cast<float>(R.GridSize.X * R.GridSize.Y);
		R.bIsMain = (Area >= Threshold);
		if (R.bIsMain)
		{
			DungeonGraph.MainRoomIndices.Add(i);
		}
	}
}

void ADungeonGenerator::Triangulate()
{
	DungeonGraph.DelaunayEdges.Reset();

	const int32 MainCount = DungeonGraph.MainRoomIndices.Num();
	if (MainCount < 3) return;

	TArray<FVector2D> Points;
	Points.Reserve(MainCount);
	for (int32 Idx : DungeonGraph.MainRoomIndices)
	{
		Points.Add(DungeonGraph.Rooms[Idx].FloatCenter);
	}

	TArray<int32> Triangles;
	DungeonTriangulate::Triangulate(Points, Triangles);

	// Triangles는 로컬 인덱스(Points 기준) → Rooms 전역 인덱스로 변환
	for (int32 t = 0; t + 2 < Triangles.Num(); t += 3)
	{
		const int32 I0 = DungeonGraph.MainRoomIndices[Triangles[t + 0]];
		const int32 I1 = DungeonGraph.MainRoomIndices[Triangles[t + 1]];
		const int32 I2 = DungeonGraph.MainRoomIndices[Triangles[t + 2]];

		DungeonGraph.DelaunayEdges.Add({ I0, I1, 0.f });
		DungeonGraph.DelaunayEdges.Add({ I1, I2, 0.f });
		DungeonGraph.DelaunayEdges.Add({ I2, I0, 0.f });
	}
}

void ADungeonGenerator::DeduplicateEdges()
{
	TSet<TPair<int32, int32>> Seen;
	TArray<FDungeonEdge> Unique;
	Unique.Reserve(DungeonGraph.DelaunayEdges.Num());

	for (FDungeonEdge& Edge : DungeonGraph.DelaunayEdges)
	{
		// 항상 작은 인덱스가 A가 되도록 정규화
		const int32 Lo = FMath::Min(Edge.RoomA, Edge.RoomB);
		const int32 Hi = FMath::Max(Edge.RoomA, Edge.RoomB);
		const TPair<int32, int32> Key(Lo, Hi);

		if (Seen.Contains(Key)) continue;
		Seen.Add(Key);

		Edge.RoomA = Lo;
		Edge.RoomB = Hi;
		Edge.Weight = FVector2D::Distance(
			DungeonGraph.Rooms[Lo].FloatCenter,
			DungeonGraph.Rooms[Hi].FloatCenter
		);
		Unique.Add(Edge);
	}

	DungeonGraph.DelaunayEdges = MoveTemp(Unique);
	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Delaunay edges after dedup: %d"), DungeonGraph.DelaunayEdges.Num());
}

void ADungeonGenerator::BuildMST()
{
	DungeonGraph.MstEdges.Reset();

	const int32 N = DungeonGraph.Rooms.Num();
	if (DungeonGraph.DelaunayEdges.Num() == 0) return;

	// Union-Find
	TArray<int32> Parent, Rank;
	Parent.SetNumUninitialized(N);
	Rank.SetNumZeroed(N);
	for (int32 i = 0; i < N; ++i) Parent[i] = i;

	TFunction<int32(int32)> Find = [&](int32 X) -> int32
	{
		if (Parent[X] != X) Parent[X] = Find(Parent[X]); // path compression
		return Parent[X];
	};

	auto Union = [&](int32 X, int32 Y) -> bool
	{
		const int32 RX = Find(X), RY = Find(Y);
		if (RX == RY) return false;
		// union by rank
		if (Rank[RX] < Rank[RY]) Parent[RX] = RY;
		else if (Rank[RX] > Rank[RY]) Parent[RY] = RX;
		else { Parent[RY] = RX; ++Rank[RX]; }
		return true;
	};

	// Weight 오름차순 정렬 후 Kruskal
	TArray<FDungeonEdge> Sorted = DungeonGraph.DelaunayEdges;
	Sorted.Sort([](const FDungeonEdge& A, const FDungeonEdge& B){ return A.Weight < B.Weight; });

	for (const FDungeonEdge& Edge : Sorted)
	{
		if (Union(Edge.RoomA, Edge.RoomB))
		{
			DungeonGraph.MstEdges.Add(Edge);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] MST edges: %d"), DungeonGraph.MstEdges.Num());
}

void ADungeonGenerator::ReviveLoops(FRandomStream& Rand)
{
	DungeonGraph.LoopEdges.Reset();

	// MST에 포함된 간선 쌍을 빠르게 조회하기 위한 Set
	TSet<TPair<int32,int32>> MstSet;
	MstSet.Reserve(DungeonGraph.MstEdges.Num());
	for (const FDungeonEdge& E : DungeonGraph.MstEdges)
	{
		MstSet.Add(TPair<int32,int32>(E.RoomA, E.RoomB));
	}

	// MST에 없는 간선 수집
	TArray<FDungeonEdge> Candidates;
	for (const FDungeonEdge& E : DungeonGraph.DelaunayEdges)
	{
		if (!MstSet.Contains(TPair<int32,int32>(E.RoomA, E.RoomB)))
		{
			Candidates.Add(E);
		}
	}

	// Fisher-Yates 셔플 후 앞에서 LoopRatio만큼 선택
	const int32 PickCount = FMath::RoundToInt(Candidates.Num() * FMath::Clamp(LoopRatio, 0.f, 1.f));
	for (int32 i = 0; i < PickCount; ++i)
	{
		const int32 j = Rand.RandRange(i, Candidates.Num() - 1);
		Candidates.Swap(i, j);
		DungeonGraph.LoopEdges.Add(Candidates[i]);
	}

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Loop edges: %d / %d candidates (LoopRatio=%.2f)"),
		DungeonGraph.LoopEdges.Num(), Candidates.Num(), LoopRatio);
}

int32 ADungeonGenerator::RunSeparation()
{
	for (int32 Iter = 0; Iter < MaxSeparationIterations; ++Iter)
	{
		if (SeparationStep() == 0)
		{
			return Iter + 1;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[DungeonGen] Separation did not converge within %d iterations"), MaxSeparationIterations);
	return MaxSeparationIterations;
}
