#include "DungeonTriangulate.h"
#include "delaunator.hpp"

void DungeonTriangulate::Triangulate(const TArray<FVector2D>& Points, TArray<int32>& OutTriangles)
{
	OutTriangles.Reset();
	if (Points.Num() < 3) return;

	std::vector<double> Coords;
	Coords.reserve(static_cast<size_t>(Points.Num()) * 2);
	for (const FVector2D& P : Points)
	{
		Coords.push_back(static_cast<double>(P.X));
		Coords.push_back(static_cast<double>(P.Y));
	}

	delaunator::Delaunator D(Coords);

	OutTriangles.Reserve(static_cast<int32>(D.triangles.size()));
	for (size_t Idx : D.triangles)
	{
		OutTriangles.Add(static_cast<int32>(Idx));
	}
}
