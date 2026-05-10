#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonStage.h"
#include "DungeonStage_ScatterRooms.generated.h"

/**
 * 1단계: 원형 영역에 방을 무작위로 흩뿌린다. 의도적으로 겹쳐도 됨.
 * 다음 단계(Separation)가 떼어낸다.
 */
UCLASS(meta=(DisplayName="1. Scatter Rooms"))
class ABYSSGEN_API UDungeonStage_ScatterRooms : public UDungeonStage
{
	GENERATED_BODY()

public:
	UDungeonStage_ScatterRooms();

	UPROPERTY(EditAnywhere, Category="Scatter", meta=(ClampMin="1"))
	int32 RoomCount = 80;

	UPROPERTY(EditAnywhere, Category="Scatter", meta=(ClampMin="0.0"))
	float SpawnRadius = 40.f;

	UPROPERTY(EditAnywhere, Category="Scatter", meta=(ClampMin="2"))
	int32 MinSize = 6;

	UPROPERTY(EditAnywhere, Category="Scatter", meta=(ClampMin="2"))
	int32 MaxSize = 12;

	virtual void Execute(FDungeonContext& Ctx) override;
	virtual void DrawDebug(const FDungeonContext& Ctx) const override;
};
