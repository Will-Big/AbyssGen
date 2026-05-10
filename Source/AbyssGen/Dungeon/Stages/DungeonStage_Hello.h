#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonStage.h"
#include "DungeonStage_Hello.generated.h"

/**
 * Week 1 검증용 더미 stage.
 * - Context.Rooms에 가짜 방 N개를 가로로 나란히 추가.
 * - DrawDebug가 각 방을 박스로 그림.
 * 이 단계가 정상 동작하면 Generator ↔ Stage ↔ Context ↔ Debug 배관이 완성된 것.
 */
UCLASS(meta=(DisplayName="Hello (Dummy)"))
class ABYSSGEN_API UDungeonStage_Hello : public UDungeonStage
{
	GENERATED_BODY()

public:
	UDungeonStage_Hello();

	UPROPERTY(EditAnywhere, Category="Hello", meta=(ClampMin="1", ClampMax="20"))
	int32 RoomCount = 3;

	UPROPERTY(EditAnywhere, Category="Hello")
	FIntPoint RoomSize = FIntPoint(6, 6);

	UPROPERTY(EditAnywhere, Category="Hello")
	float SpacingCells = 8.f;

	UPROPERTY(EditAnywhere, Category="Hello")
	FColor DebugColor = FColor::Cyan;

	virtual void Execute(FDungeonContext& Ctx) override;
	virtual void DrawDebug(const FDungeonContext& Ctx) const override;
};
