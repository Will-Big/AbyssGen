#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.h"

namespace DungeonCorridorGen
{
	void ComputeCorridors(FDungeonGraph& Graph, FRandomStream& Rand);
	void MarkDoorCells(FDungeonGraph& Graph);
}
