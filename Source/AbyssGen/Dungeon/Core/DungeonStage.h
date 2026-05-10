#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DungeonStage.generated.h"

struct FDungeonContext;

/**
 * 모든 던전 생성 단계의 추상 베이스.
 *
 * 핵심 규칙:
 *  - 서브클래스는 Execute()에서 Context를 갱신한다.
 *  - 서브클래스는 DrawDebug()에서 자신이 만든 결과를 시각화한다(Step 모드에서만 호출됨).
 *  - 외부 상태(엔진, 다른 액터)에 접근하지 않는다 — 오직 Context.
 *
 * UCLASS 메타:
 *  - Abstract        : 직접 인스턴스화 불가. 서브클래스만 가능.
 *  - EditInlineNew   : 다른 객체의 UPROPERTY(Instanced) 안에서 인라인 생성 가능.
 *                       즉 ADungeonGenerator의 Stages 배열에서 드롭다운으로 추가됨.
 *  - CollapseCategories : 디테일 패널을 깔끔하게.
 */
UCLASS(Abstract, EditInlineNew, BlueprintType, Blueprintable, CollapseCategories)
class ABYSSGEN_API UDungeonStage : public UObject
{
	GENERATED_BODY()

public:
	/** 비활성화 시 파이프라인이 이 stage를 건너뜀. */
	UPROPERTY(EditAnywhere, Category="Stage")
	bool bEnabled = true;

	/** 디테일 패널과 viewport HUD에 표시될 이름. 비어 있으면 클래스 이름 사용. */
	UPROPERTY(EditAnywhere, Category="Stage")
	FString DisplayName;

	/** 이 단계가 무엇을 하는지 한국어 한두 줄 설명. Step 모드에서 HUD로 노출 예정. */
	UPROPERTY(EditAnywhere, Category="Stage", meta=(MultiLine="true"))
	FString Summary;

	/** 메인 작업: Context 갱신. */
	virtual void Execute(FDungeonContext& Ctx) PURE_VIRTUAL(UDungeonStage::Execute, );

	/** 에디터 Step 모드에서 호출. stage가 자기가 만든 결과를 그린다. */
	virtual void DrawDebug(const FDungeonContext& Ctx) const {}

	/** 표시 이름이 비어 있으면 클래스 이름을 반환. */
	FString GetEffectiveDisplayName() const
	{
		return DisplayName.IsEmpty() ? GetClass()->GetName() : DisplayName;
	}
};
