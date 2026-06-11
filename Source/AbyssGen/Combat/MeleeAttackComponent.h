// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/TimerHandle.h"
#include "MeleeAttackComponent.generated.h"

/** 공격 1회가 끝났을 때 (몽타주 종료 또는 폴백 시간 경과) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeleeAttackFinishedSignature);

class ACharacter;

/**
 *  근접 공격 1회를 수행하는 재사용 컴포넌트.
 *  몽타주 재생 → 일정 시간 후 전방 스윕 트레이스 → 대상에 데미지 → 종료 통지.
 *  대상 탐색/판정은 호출자가 담당하고, 이 컴포넌트는 "공격 실행"만 책임진다.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UMeleeAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeAttackComponent();

	/** Target을 향해 공격을 시작한다. 시작되면 true. */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool StartAttack(AActor* Target);

	/** 진행 중인 공격을 즉시 취소한다(타이머 정리, 종료 통지 없음). */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void CancelAttack(bool bBroadcastFinished = false);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ExecuteAttackTrace();

	UFUNCTION(BlueprintPure, Category = "Attack")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintPure, Category = "Attack")
	float GetAttackPrepareDuration() const { return AttackPrepareDuration; }

	/** 공격 1회 종료 통지 */
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnMeleeAttackFinishedSignature OnAttackFinished;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackDamage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackTraceDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackTraceRadius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "s"))
	float AttackPrepareDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "s"))
	float AttackFallbackDuration = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bRequireLineOfSightOnHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TEnumAsByte<ECollisionChannel> LineOfSightTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Debug")
	bool bDrawAttackDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Debug", meta = (ClampMin = "0.0", Units = "s"))
	float AttackDebugDrawTime = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;

	bool bIsAttacking = false;

	FOnMontageEnded AttackMontageEndedDelegate;

	FTimerHandle AttackFinishTimer;

	void PerformMeleeAttackTrace();

	void FinishAttack();

	void HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void FaceTarget();

	bool HasLineOfSightToCurrentTarget() const;
};
