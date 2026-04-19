#include "DungeonGenerator.h"
#include "DungeonDebugDraw.h"
#include "DungeonISMSpawner.h"
#include "DrawDebugHelpers.h"
#include "DungeonRoomGen.h"
#include "DungeonGraphBuilder.h"
#include "DungeonCorridorGen.h"
#include "DungeonRoomClassifier.h"
#include "DungeonConnectivity.h"
#include "DungeonLog.h"
#include "Components/InstancedStaticMeshComponent.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	FloorISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorISM"));
	SetRootComponent(FloorISM);

	WallISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallISM"));
	WallISM->SetupAttachment(FloorISM);
}

void ADungeonGenerator::Generate()
{
	DungeonGraph.Reset();

	if (Config.bRandomizeSeed)
		Config.Seed = FMath::Rand();
	DungeonGraph.Seed = Config.Seed;

	FRandomStream Rand(Config.Seed);

	DungeonRoomGen::GenerateRooms(DungeonGraph, Config, Rand);
	const int32 Iterations = DungeonRoomGen::RunSeparation(DungeonGraph, Config);
	DungeonGraphBuilder::SelectMainRooms(DungeonGraph, Config);
	DungeonGraphBuilder::Triangulate(DungeonGraph);
	DungeonGraphBuilder::DeduplicateEdges(DungeonGraph);
	DungeonGraphBuilder::BuildMST(DungeonGraph);
	DungeonGraphBuilder::ReviveLoops(DungeonGraph, Config, Rand);
	DungeonCorridorGen::ComputeCorridors(DungeonGraph, Rand);
	DungeonCorridorGen::MarkDoorCells(DungeonGraph);
	DungeonRoomClassifier::AssignSpecialRooms(DungeonGraph, Rand);
	DungeonConnectivity::Verify(DungeonGraph);

	if (FloorMesh)
	{
		FloorISM->SetStaticMesh(FloorMesh);
		DungeonISMSpawner::SpawnFloorTiles(FloorISM, DungeonGraph);
	}
	else
	{
		FloorISM->ClearInstances();
		UE_LOG(LogDungeonGen, Warning, TEXT("FloorMesh not assigned"));
	}

	if (WallMesh)
	{
		WallISM->SetStaticMesh(WallMesh);
		DungeonISMSpawner::SpawnWallTiles(WallISM, DungeonGraph);
	}
	else
	{
		WallISM->ClearInstances();
		UE_LOG(LogDungeonGen, Warning, TEXT("WallMesh not assigned"));
	}

	if (UWorld* World = GetWorld())
		FlushPersistentDebugLines(World);
	FDungeonDebugDraw::DrawGraph(GetWorld(), DungeonGraph);

	UE_LOG(LogDungeonGen, Log, TEXT("Generate() Rooms=%d Main=%d Seed=%d SepIters=%d"),
		DungeonGraph.Rooms.Num(), DungeonGraph.MainRoomIndices.Num(), Config.Seed, Iterations);
}

void ADungeonGenerator::ClearFloor()
{
	FloorISM->ClearInstances();
	WallISM->ClearInstances();
}

void ADungeonGenerator::StepSeparation()
{
	const int32 Overlapping = DungeonRoomGen::SeparationStep(DungeonGraph, Config);

	if (UWorld* World = GetWorld())
		FlushPersistentDebugLines(World);
	FDungeonDebugDraw::DrawGraph(GetWorld(), DungeonGraph);

	UE_LOG(LogDungeonGen, Log, TEXT("SeparationStep overlapping pairs=%d"), Overlapping);
}
