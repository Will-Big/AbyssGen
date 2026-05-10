#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "DungeonTypes.h"

class UWorld;

/**
 * 파이프라인의 모든 stage가 공유하는 작업판.
 * - 일반 C++ 구조체이며 UStruct가 아님 — 비-UPROPERTY 멤버(FRandomStream, UWorld*)를 자유롭게 보관.
 * - 한 번의 Generate 호출 동안만 의미가 있다(transient).
 * - stage는 이 구조체를 읽고/쓰는 것 외에는 외부 상태를 건드리지 않는다.
 */
struct FDungeonContext
{
	UWorld* World = nullptr;
	int32 Seed = 0;
	FRandomStream Rng;

	TArray<FDungeonRoom> Rooms;

	/** 메인 방 인덱스 목록 (Rooms 내). Separation 단계에서 채워짐. */
	TArray<int32> MainRoomIndices;

	/** Triangulate가 만든 후보 간선들. 메인 방 인덱스 기준. */
	TArray<FDungeonEdge> DelaunayEdges;

	/** MST가 선택한 뼈대 간선들. */
	TArray<FDungeonEdge> MstEdges;

	/** 루프 복원으로 다시 추가된 간선들. */
	TArray<FDungeonEdge> LoopEdges;

	/** L자 복도들. 각 활성 간선당 1개. */
	TArray<FLCorridor> Corridors;

	int32 ExecutedStageCount = 0;

	void Reset(int32 InSeed, UWorld* InWorld)
	{
		World = InWorld;
		Seed = InSeed;
		Rng.Initialize(InSeed);
		Rooms.Reset();
		MainRoomIndices.Reset();
		DelaunayEdges.Reset();
		MstEdges.Reset();
		LoopEdges.Reset();
		Corridors.Reset();
		ExecutedStageCount = 0;
	}
};
