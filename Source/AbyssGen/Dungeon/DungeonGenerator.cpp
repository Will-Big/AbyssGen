#include "DungeonGenerator.h"

#include "Core/DungeonStage.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

int32 ADungeonGenerator::ResolveSeed() const
{
	if (bRandomizeSeed)
	{
		return FMath::Rand();
	}
	return Seed;
}

void ADungeonGenerator::EnsureContextSeeded()
{
	if (Context.World != GetWorld() || NextStageIndex == 0)
	{
		const int32 ResolvedSeed = ResolveSeed();
		Context.Reset(ResolvedSeed, GetWorld());
		// 사용자가 다음 Generate에서 같은 결과를 재현하려면 Seed를 보라고 표시.
		Seed = ResolvedSeed;
		NextStageIndex = 0;
		LastExecutedStageName.Reset();
	}
}

void ADungeonGenerator::ResetPipeline()
{
	if (UWorld* World = GetWorld())
	{
		FlushPersistentDebugLines(World);
	}
	Context.Reset(0, GetWorld());
	NextStageIndex = 0;
	LastExecutedStageName.Reset();
}

void ADungeonGenerator::RandomizeSeedNow()
{
	Seed = FMath::Rand();
	bRandomizeSeed = false;
}

void ADungeonGenerator::ExecuteStageAt(int32 Index, bool bDrawDebug)
{
	if (!Stages.IsValidIndex(Index)) return;
	UDungeonStage* Stage = Stages[Index];
	if (!Stage || !Stage->bEnabled) return;

	Stage->Execute(Context);
	Context.ExecutedStageCount++;
	LastExecutedStageName = Stage->GetEffectiveDisplayName();

	if (bDrawDebug)
	{
		Stage->DrawDebug(Context);
	}
}

void ADungeonGenerator::StepOnce()
{
	EnsureContextSeeded();

	while (NextStageIndex < Stages.Num())
	{
		UDungeonStage* Stage = Stages[NextStageIndex];
		if (Stage && Stage->bEnabled)
		{
			ExecuteStageAt(NextStageIndex, /*bDrawDebug*/ true);
			NextStageIndex++;
			return;
		}
		// 비활성/널은 건너뛰고 다음을 시도.
		NextStageIndex++;
	}
}

void ADungeonGenerator::RunAll()
{
	ResetPipeline();
	EnsureContextSeeded();

	for (int32 i = 0; i < Stages.Num(); ++i)
	{
		ExecuteStageAt(i, /*bDrawDebug*/ true);
	}
	NextStageIndex = Stages.Num();
}

void ADungeonGenerator::GenerateImmediate()
{
	// 런타임 경로. 디버그 그리지 않음.
	if (UWorld* World = GetWorld())
	{
		FlushPersistentDebugLines(World);
	}
	Context.Reset(ResolveSeed(), GetWorld());

	for (int32 i = 0; i < Stages.Num(); ++i)
	{
		ExecuteStageAt(i, /*bDrawDebug*/ false);
	}
	NextStageIndex = Stages.Num();
}
