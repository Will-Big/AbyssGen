#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonStage.h"
#include "DungeonStage_MstAndLoops.generated.h"

/**
 * 4단계: 후보 간선 중에서 모든 메인 방을 연결하는 최소 신장 트리(MST)를 고르고,
 * 남은 간선 중 일부를 되살려 루프를 만든다.
 *
 * Kruskal + Union-Find. LoopRatio로 트리(0) ↔ 미로(1) 사이 조절.
 */
UCLASS(meta=(DisplayName="4. MST + Loops"))
class ABYSSGEN_API UDungeonStage_MstAndLoops : public UDungeonStage
{
	GENERATED_BODY()

public:
	UDungeonStage_MstAndLoops();

	UPROPERTY(EditAnywhere, Category="MST", meta=(ClampMin="0.0", ClampMax="1.0"))
	float LoopRatio = 0.15f;

	virtual void Execute(FDungeonContext& Ctx) override;
	virtual void DrawDebug(const FDungeonContext& Ctx) const override;
};
