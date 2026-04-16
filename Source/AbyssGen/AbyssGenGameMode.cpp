// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbyssGenGameMode.h"
#include "delaunator.hpp"

AAbyssGenGameMode::AAbyssGenGameMode()
{
	// stub
}

void AAbyssGenGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Day 2 검증: 더미 점 5개로 Delaunay 삼각분할
	std::vector<double> Coords = {
		0.0,  0.0,   // 0
		4.0,  0.0,   // 1
		2.0,  3.0,   // 2
		1.0,  5.0,   // 3
		5.0,  4.0,   // 4
	};

	delaunator::Delaunator D(Coords);

	UE_LOG(LogTemp, Log, TEXT("[Delaunator] 점 %d개 → 삼각형 %d개"),
		(int)(Coords.size() / 2),
		(int)(D.triangles.size() / 3));

	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		UE_LOG(LogTemp, Log, TEXT("  Triangle: %d - %d - %d"),
			(int)D.triangles[i],
			(int)D.triangles[i + 1],
			(int)D.triangles[i + 2]);
	}
}
