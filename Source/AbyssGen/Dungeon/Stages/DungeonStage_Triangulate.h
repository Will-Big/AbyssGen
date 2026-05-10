#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonStage.h"
#include "DungeonStage_Triangulate.generated.h"

/**
 * 3단계: 메인 방들의 중심점을 Delaunay 삼각분할해 후보 간선들을 만든다.
 * 후보 간선 = 다음 단계(MST)의 입력. 알고리즘은 외부 라이브러리(delaunator-cpp).
 */
UCLASS(meta=(DisplayName="3. Triangulate"))
class ABYSSGEN_API UDungeonStage_Triangulate : public UDungeonStage
{
	GENERATED_BODY()

public:
	UDungeonStage_Triangulate();

	virtual void Execute(FDungeonContext& Ctx) override;
	virtual void DrawDebug(const FDungeonContext& Ctx) const override;
};
