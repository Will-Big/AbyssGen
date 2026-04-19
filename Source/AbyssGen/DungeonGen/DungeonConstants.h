#pragma once

#include "CoreMinimal.h"

namespace DungeonConstants
{
	// 1 grid cell당 Unreal Unit(cm)
	constexpr float CellSize = 200.f;

	// 방 크기 샘플링 범위 (grid cells, inclusive)
	constexpr int32 MinRoomSize = 5;
	constexpr int32 MaxRoomSize = 8;
}
