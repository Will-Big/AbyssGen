// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContactDamageComponent.generated.h"

class UPrimitiveComponent;

/**
 *  지정한 충돌 컴포넌트에 겹친 Pawn에게 주기적으로 피해를 주는 재사용 컴포넌트.
 *  대상별 재타격 쿨다운(ReHitInterval)을 두어 매 프레임 피해를 막는다.
 *  엔진 표준 ApplyDamage를 사용하므로 대상의 HealthComponent가 피해를 수신한다.
 */
UCLASS(ClassGroup = (Trap), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UContactDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UContactDamageComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 겹침을 감시할 충돌 컴포넌트를 지정한다(미지정 시 소유 액터 루트). */
	void SetDamageCollision(UPrimitiveComponent* InComponent) { DamageCollision = InComponent; }

	//~ 비활성화 시 재타격 기록 초기화
	virtual void Deactivate() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Contact Damage", meta = (ClampMin = "0.0"))
	float Damage = 1.0f;

	/** 같은 대상을 다시 때리기까지의 최소 간격(초). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Contact Damage", meta = (ClampMin = "0.0", Units = "s"))
	float ReHitInterval = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> DamageCollision;

	UPROPERTY()
	TMap<TObjectPtr<AActor>, float> LastHitTimes;
};
