// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

class UHealthComponent;

UINTERFACE(MinimalAPI, BlueprintType)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 *  전투에서 데미지를 받을 수 있는 액터의 계약.
 *  데미지 전달 자체는 엔진 표준 ApplyDamage/TakeDamage 경로를 쓰고,
 *  이 인터페이스는 체력 상태를 다형적으로 조회하는 용도다(AI 타겟 검증, UI 등).
 */
class IDamageable
{
	GENERATED_BODY()

public:
	/** 이 액터의 체력 컴포넌트를 반환한다(없으면 nullptr). */
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	UHealthComponent* GetHealthComponent() const;
};
