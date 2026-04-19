#include "DungeonRoomGen.h"
#include "DungeonConstants.h"
#include "DungeonLog.h"

void DungeonRoomGen::GenerateRooms(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config, FRandomStream& Rand)
{
	Graph.Rooms.Reserve(Config.TotalRoomCount);

	for (int32 i = 0; i < Config.TotalRoomCount; ++i)
	{
		const float R     = Config.SpawnRadius * FMath::Sqrt(Rand.GetFraction());
		const float Theta = Rand.GetFraction() * 2.f * PI;

		FRoom Room;
		Room.FloatCenter = FVector2D(R * FMath::Cos(Theta), R * FMath::Sin(Theta));
		Room.GridCenter  = FIntPoint(FMath::RoundToInt(Room.FloatCenter.X), FMath::RoundToInt(Room.FloatCenter.Y));
		Room.GridSize    = FIntPoint(
			Rand.RandRange(DungeonConstants::MinRoomSize, DungeonConstants::MaxRoomSize),
			Rand.RandRange(DungeonConstants::MinRoomSize, DungeonConstants::MaxRoomSize));

		Graph.Rooms.Add(Room);
	}
}

int32 DungeonRoomGen::SeparationStep(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config)
{
	const int32 N = Graph.Rooms.Num();
	TArray<FVector2D> Pushes;
	Pushes.SetNumZeroed(N);

	int32 Overlapping = 0;

	for (int32 i = 0; i < N; ++i)
	{
		for (int32 j = i + 1; j < N; ++j)
		{
			const FRoom& A = Graph.Rooms[i];
			const FRoom& B = Graph.Rooms[j];

			const FVector2D Delta = A.FloatCenter - B.FloatCenter;
			const float MinSepX = (A.GridSize.X + B.GridSize.X) * 0.5f + Config.SeparationPadding;
			const float MinSepY = (A.GridSize.Y + B.GridSize.Y) * 0.5f + Config.SeparationPadding;

			if (FMath::Abs(Delta.X) >= MinSepX) continue;
			if (FMath::Abs(Delta.Y) >= MinSepY) continue;

			++Overlapping;

			FVector2D Dir = Delta;
			if (Dir.IsNearlyZero()) Dir = FVector2D(1.f, 0.f);
			else Dir.Normalize();

			Pushes[i] += Dir;
			Pushes[j] -= Dir;
		}
	}

	for (int32 i = 0; i < N; ++i)
	{
		FRoom& R = Graph.Rooms[i];
		R.FloatCenter += Pushes[i];
		R.GridCenter   = FIntPoint(FMath::RoundToInt(R.FloatCenter.X), FMath::RoundToInt(R.FloatCenter.Y));
	}

	return Overlapping;
}

int32 DungeonRoomGen::RunSeparation(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config)
{
	for (int32 Iter = 0; Iter < Config.MaxSeparationIterations; ++Iter)
	{
		if (SeparationStep(Graph, Config) == 0)
			return Iter + 1;
	}

	UE_LOG(LogDungeonGen, Warning, TEXT("Separation did not converge within %d iterations"), Config.MaxSeparationIterations);
	return Config.MaxSeparationIterations;
}
