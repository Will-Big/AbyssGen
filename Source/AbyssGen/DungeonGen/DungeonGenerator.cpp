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
	ComputeCorridors(Rand);
	MarkDoorCells();
	AssignSpecialRooms(Rand);
	VerifyConnectivity();

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

void ADungeonGenerator::ComputeCorridors(FRandomStream& Rand)
{
	DungeonGraph.Corridors.Reset();

	// MST + Loop 간선 모두 복도로 연결
	TArray<FDungeonEdge> AllEdges = DungeonGraph.MstEdges;
	AllEdges.Append(DungeonGraph.LoopEdges);

	for (const FDungeonEdge& Edge : AllEdges)
	{
		const FIntPoint A = DungeonGraph.Rooms[Edge.RoomA].GridCenter;
		const FIntPoint B = DungeonGraph.Rooms[Edge.RoomB].GridCenter;

		// 수평 먼저(0) vs 수직 먼저(1) 랜덤 선택
		FLCorridor Corridor;
		Corridor.RoomA  = Edge.RoomA;
		Corridor.RoomB  = Edge.RoomB;
		Corridor.Start  = A;
		Corridor.End    = B;
		Corridor.Corner = Rand.RandRange(0, 1) == 0
			? FIntPoint(B.X, A.Y)  // 수평 먼저
			: FIntPoint(A.X, B.Y); // 수직 먼저

		DungeonGraph.Corridors.Add(Corridor);
	}

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Corridors: %d"), DungeonGraph.Corridors.Num());
}

void ADungeonGenerator::MarkDoorCells()
{
	for (FRoom& R : DungeonGraph.Rooms) R.DoorCells.Reset();

	auto IsInsideRoom = [](const FIntPoint& Cell, const FRoom& Room) -> bool
	{
		const int32 HalfX = Room.GridSize.X / 2;
		const int32 HalfY = Room.GridSize.Y / 2;
		return Cell.X >= Room.GridCenter.X - HalfX && Cell.X < Room.GridCenter.X + HalfX
			&& Cell.Y >= Room.GridCenter.Y - HalfY && Cell.Y < Room.GridCenter.Y + HalfY;
	};

	// 이전→현재 전환점(진입/탈출)만 DoorCell로 마킹
	auto ScanSegment = [&](FIntPoint From, FIntPoint To)
	{
		const int32 DX = FMath::Sign(To.X - From.X);
		const int32 DY = FMath::Sign(To.Y - From.Y);
		FIntPoint Prev = From;
		FIntPoint Cell = From;
		Cell.X += DX;
		Cell.Y += DY;

		while (Prev != To)
		{
			for (int32 i = 0; i < DungeonGraph.Rooms.Num(); ++i)
			{
				FRoom& R = DungeonGraph.Rooms[i];
				const bool bPrevInside = IsInsideRoom(Prev, R);
				const bool bCurrInside = IsInsideRoom(Cell, R);

				// 외부→내부(진입): Cell이 도어
				// 내부→외부(탈출): Prev가 도어
				if (!bPrevInside && bCurrInside && !R.DoorCells.Contains(Cell))
				{
					R.DoorCells.Add(Cell);
				}
				else if (bPrevInside && !bCurrInside && !R.DoorCells.Contains(Prev))
				{
					R.DoorCells.Add(Prev);
				}
			}
			Prev = Cell;
			Cell.X += DX;
			Cell.Y += DY;
		}
	};

	for (const FLCorridor& C : DungeonGraph.Corridors)
	{
		ScanSegment(C.Start, C.Corner);
		ScanSegment(C.Corner, C.End);
	}

	int32 TotalDoors = 0;
	for (const FRoom& R : DungeonGraph.Rooms) TotalDoors += R.DoorCells.Num();
	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Door cells marked: %d"), TotalDoors);
}

void ADungeonGenerator::AssignSpecialRooms(FRandomStream& Rand)
{
	// 모든 메인 방 Normal로 초기화
	for (int32 Idx : DungeonGraph.MainRoomIndices)
	{
		DungeonGraph.Rooms[Idx].Type = ERoomType::Normal;
	}

	if (DungeonGraph.MainRoomIndices.IsEmpty()) return;

	// Start: 원점에 가장 가까운 메인 방
	int32 StartIdx = DungeonGraph.MainRoomIndices[0];
	float MinDist = DungeonGraph.Rooms[StartIdx].FloatCenter.SizeSquared();
	for (int32 Idx : DungeonGraph.MainRoomIndices)
	{
		const float D = DungeonGraph.Rooms[Idx].FloatCenter.SizeSquared();
		if (D < MinDist) { MinDist = D; StartIdx = Idx; }
	}
	DungeonGraph.Rooms[StartIdx].Type = ERoomType::Start;

	// Boss: Start에서 FloatCenter 거리가 가장 먼 메인 방
	int32 BossIdx = StartIdx;
	float MaxDist = 0.f;
	const FVector2D StartCenter = DungeonGraph.Rooms[StartIdx].FloatCenter;
	for (int32 Idx : DungeonGraph.MainRoomIndices)
	{
		if (Idx == StartIdx) continue;
		const float D = FVector2D::DistSquared(DungeonGraph.Rooms[Idx].FloatCenter, StartCenter);
		if (D > MaxDist) { MaxDist = D; BossIdx = Idx; }
	}
	DungeonGraph.Rooms[BossIdx].Type = ERoomType::Boss;

	// MST에서 연결 수(degree) 계산
	TMap<int32, int32> Degree;
	for (int32 Idx : DungeonGraph.MainRoomIndices) Degree.Add(Idx, 0);
	for (const FDungeonEdge& E : DungeonGraph.MstEdges)
	{
		++Degree[E.RoomA];
		++Degree[E.RoomB];
	}

	// Treasure: Start·Boss가 아닌 MST leaf(degree==1) 중 랜덤
	TArray<int32> Leaves;
	for (int32 Idx : DungeonGraph.MainRoomIndices)
	{
		if (Idx == StartIdx || Idx == BossIdx) continue;
		if (Degree.FindRef(Idx) == 1) Leaves.Add(Idx);
	}

	if (!Leaves.IsEmpty())
	{
		const int32 TreasureIdx = Leaves[Rand.RandRange(0, Leaves.Num() - 1)];
		DungeonGraph.Rooms[TreasureIdx].Type = ERoomType::Treasure;
	}

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Special rooms — Start:%d Boss:%d Leaves:%d"),
		StartIdx, BossIdx, Leaves.Num());
}

void ADungeonGenerator::VerifyConnectivity() const
{
	if (DungeonGraph.MainRoomIndices.IsEmpty()) return;

	// 인접 리스트 구성 (MST + Loop)
	TMap<int32, TArray<int32>> Adj;
	for (int32 Idx : DungeonGraph.MainRoomIndices) Adj.Add(Idx, {});

	auto AddEdge = [&](const FDungeonEdge& E)
	{
		if (Adj.Contains(E.RoomA)) Adj[E.RoomA].Add(E.RoomB);
		if (Adj.Contains(E.RoomB)) Adj[E.RoomB].Add(E.RoomA);
	};
	for (const FDungeonEdge& E : DungeonGraph.MstEdges)  AddEdge(E);
	for (const FDungeonEdge& E : DungeonGraph.LoopEdges)  AddEdge(E);

	// Start 방에서 BFS
	int32 StartIdx = INDEX_NONE;
	for (int32 Idx : DungeonGraph.MainRoomIndices)
	{
		if (DungeonGraph.Rooms[Idx].Type == ERoomType::Start) { StartIdx = Idx; break; }
	}
	if (StartIdx == INDEX_NONE) StartIdx = DungeonGraph.MainRoomIndices[0];

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

	const int32 Total = DungeonGraph.MainRoomIndices.Num();
	const int32 Reached = Visited.Num();

	if (Reached == Total)
	{
		UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Connectivity OK — all %d main rooms reachable"), Total);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGen] Connectivity FAIL — reached %d / %d main rooms"), Reached, Total);
	}
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
