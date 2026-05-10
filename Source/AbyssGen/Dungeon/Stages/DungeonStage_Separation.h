#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonStage.h"
#include "DungeonStage_Separation.generated.h"

/**
 * 2단계: 겹친 방들을 서로 밀어내고, 충분히 큰 방을 "메인 방"으로 표시한다.
 *
 * 알고리즘은 단순한 AABB 분리 스티어링: 매 반복마다 겹친 쌍을 찾아 서로 반대 방향으로 미세하게 민다.
 * 한 번의 반복 안에서 모든 방을 한 번씩만 처리하므로 안정적이지만 느릴 수 있다.
 */
UCLASS(meta=(DisplayName="2. Separation"))
class ABYSSGEN_API UDungeonStage_Separation : public UDungeonStage
{
	GENERATED_BODY()

public:
	UDungeonStage_Separation();

	UPROPERTY(EditAnywhere, Category="Separation", meta=(ClampMin="1"))
	int32 MaxIterations = 500;

	UPROPERTY(EditAnywhere, Category="Separation", meta=(ClampMin="0.0"))
	float Padding = 1.f;

	/** 메인 방 판정 기준: 평균 크기 × 이 값 이상이면 메인. */
	UPROPERTY(EditAnywhere, Category="Separation", meta=(ClampMin="1.0"))
	float MainRoomSizeMultiplier = 1.25f;

	virtual void Execute(FDungeonContext& Ctx) override;
	virtual void DrawDebug(const FDungeonContext& Ctx) const override;
};
