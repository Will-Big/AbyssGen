#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/DungeonContext.h"
#include "DungeonGenerator.generated.h"

class UDungeonStage;

/**
 * 던전 생성기 액터. 레벨에 하나 놓고 디테일 패널에서 stage 파이프라인을 조립한다.
 *
 * 실행 모드:
 *  - RunAll          : 모든 stage를 순차 즉시 실행. 런타임 GenerateImmediate()와 같은 경로.
 *  - StepOnce        : 한 단계만 실행하고 그 단계의 DrawDebug()를 호출(에디터 학습용).
 *  - ResetPipeline   : Context를 비우고 디버그 라인 플러시.
 */
UCLASS()
class ABYSSGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGenerator();

	// ---- Config ----
	UPROPERTY(EditAnywhere, Category="Dungeon|Config")
	int32 Seed = 42;

	UPROPERTY(EditAnywhere, Category="Dungeon|Config")
	bool bRandomizeSeed = true;

	/**
	 * 파이프라인 단계 배열. 위에서부터 아래로 순서대로 실행된다.
	 * Instanced + EditInlineNew 조합으로 디테일 패널에서 드롭다운으로 서브클래스를 추가한다.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category="Dungeon|Pipeline")
	TArray<TObjectPtr<UDungeonStage>> Stages;

	// ---- Status (read-only) ----
	UPROPERTY(VisibleAnywhere, Category="Dungeon|Status")
	int32 NextStageIndex = 0;

	UPROPERTY(VisibleAnywhere, Category="Dungeon|Status")
	FString LastExecutedStageName;

	// ---- Editor actions ----
	UFUNCTION(CallInEditor, Category="Dungeon|Actions")
	void RunAll();

	UFUNCTION(CallInEditor, Category="Dungeon|Actions")
	void StepOnce();

	UFUNCTION(CallInEditor, Category="Dungeon|Actions")
	void ResetPipeline();

	UFUNCTION(CallInEditor, Category="Dungeon|Actions")
	void RandomizeSeedNow();

	/** 런타임용. 한 호출에 모든 stage를 즉시 실행하고 디버그 그리지 않는다. */
	void GenerateImmediate();

protected:
	/** 한 번의 Generate 동안만 유효한 작업판. UPROPERTY 아님. */
	FDungeonContext Context;

	void EnsureContextSeeded();
	int32 ResolveSeed() const;
	void ExecuteStageAt(int32 Index, bool bDrawDebug);
};
