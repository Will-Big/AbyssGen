#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.h"

class UInstancedStaticMeshComponent;

namespace DungeonISMSpawner
{
	void SpawnFloorTiles(UInstancedStaticMeshComponent* ISM, const FDungeonGraph& Graph);
	void SpawnWallTiles(UInstancedStaticMeshComponent* ISM, const FDungeonGraph& Graph);
}
