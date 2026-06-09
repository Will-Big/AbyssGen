// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/** 체력 변화 시 (현재 체력, 최대 체력) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);

/** 사망 시 (가해자) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DamageCauser);

/**
 *  체력/데미지/사망을 캡슐화하는 재사용 컴포넌트.
 *  소유 액터의 OnTakeAnyDamage(엔진 표준 데미지 경로)에 자동으로 연결되므로,
 *  소유 액터는 TakeDamage를 오버라이드할 필요가 없다.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	/** 데미지를 적용하고 실제 감소량을 반환한다. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float ApplyDamage(float DamageAmount, AActor* DamageCauser);

	/** 체력을 회복한다. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	/** 즉시 사망 처리한다. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Kill(AActor* DamageCauser);

	/** 최대 체력을 설정한다(bRefill이면 현재 체력도 가득 채움). */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float NewMaxHealth, bool bRefill = true);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return !bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	/** 체력 변화 이벤트 (UI/연출이 구독) */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	/** 사망 이벤트 (소유 액터/연출이 구독) */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	/** 소유 액터의 OnTakeAnyDamage에 연결되어 엔진 데미지를 받는다. */
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
