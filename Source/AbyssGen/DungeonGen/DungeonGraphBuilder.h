#pragma once

#include "CoreMinimal.h"
#include "DungeonTypes.h"

namespace DungeonGraphBuilder
{
	void SelectMainRooms(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config);
	void Triangulate(FDungeonGraph& Graph);
	void DeduplicateEdges(FDungeonGraph& Graph);
	void BuildMST(FDungeonGraph& Graph);
	void ReviveLoops(FDungeonGraph& Graph, const FDungeonGeneratorConfig& Config, FRandomStream& Rand);
}
