#pragma once

#include "CoreMinimal.h"

namespace DungeonTriangulate
{
	// Points (x,y 좌표)를 Delaunay 삼각분할. OutTriangles는 3개 인덱스씩 묶인 배열.
	ABYSSGEN_API void Triangulate(const TArray<FVector2D>& Points, TArray<int32>& OutTriangles);
}
