#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonStage.h"
#include "DungeonStage_LCorridors.generated.h"

/**
 * 5단계: 활성 간선(MST + Loop) 각각에 대해 L자 복도 경로를 만든다.
 * 결과는 Ctx.Corridors. 다음 Week의 ISM 스폰 단계가 이 경로를 따라 큐브를 배치한다.
 */
UCLASS(meta=(DisplayName="5. L Corridors"))
class ABYSSGEN_API UDungeonStage_LCorridors : public UDungeonStage
{
	GENERATED_BODY()

public:
	UDungeonStage_LCorridors();

	virtual void Execute(FDungeonContext& Ctx) override;
	virtual void DrawDebug(const FDungeonContext& Ctx) const override;
};
