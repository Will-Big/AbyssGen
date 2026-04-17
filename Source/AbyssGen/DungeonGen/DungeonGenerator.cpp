#include "DungeonGenerator.h"
#include "DungeonDebugDraw.h"
#include "DrawDebugHelpers.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADungeonGenerator::Generate()
{
	DungeonGraph.Reset();
	DungeonGraph.Seed = Seed;

	FRandomStream Rand(Seed);

	GenerateRooms(Rand);
	const int32 Iterations = RunSeparation();
	SelectMainRooms();

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
			const float MinSepX = (A.GridSize.X + B.GridSize.X) * 0.5f;
			const float MinSepY = (A.GridSize.Y + B.GridSize.Y) * 0.5f;

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
