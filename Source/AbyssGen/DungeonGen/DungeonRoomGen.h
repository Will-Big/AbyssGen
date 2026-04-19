#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.h"

namespace DungeonRoomGen
{
	void GenerateRooms(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config, FRandomStream& Rand);
	int32 SeparationStep(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config);
	int32 RunSeparation(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config);
}
