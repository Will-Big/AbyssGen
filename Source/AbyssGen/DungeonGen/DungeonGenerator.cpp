#include "DungeonGenerator.h"
#include "DungeonDebugDraw.h"

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

	FDungeonDebugDraw::DrawGraph(GetWorld(), DungeonGraph);

	UE_LOG(LogTemp, Log, TEXT("[DungeonGen] Generate() called. Rooms=%d, Seed=%d"), DungeonGraph.Rooms.Num(), Seed);
}

void ADungeonGenerator::GenerateRooms(FRandomStream& Rand)
{
	DungeonGraph.Rooms.Reserve(TotalRoomCount);

	for (int32 i = 0; i < TotalRoomCount; ++i)
	{
		// 원형 균등 분포: r = R * sqrt(rand) 로 면적 균등
		const float R     = SpawnRadius * FMath::Sqrt(Rand.GetFraction());
		const float Theta = Rand.GetFraction() * 2.f * PI;

		FRoom Room;
		Room.FloatCenter = FVector2D(R * FMath::Cos(Theta), R * FMath::Sin(Theta));
		Room.GridCenter  = FIntPoint(FMath::RoundToInt(Room.FloatCenter.X), FMath::RoundToInt(Room.FloatCenter.Y));
		Room.GridSize    = FIntPoint(Rand.RandRange(6, 12), Rand.RandRange(6, 12));

		DungeonGraph.Rooms.Add(Room);
	}
}
