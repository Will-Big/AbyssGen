// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TimerHandle.h"
#include "Spawnable.h"
#include "TrapHazard.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UPrimitiveComponent;
class UOscillatingMovementComponent;
class UContactDamageComponent;

UENUM(BlueprintType)
enum class ETrapState : uint8
{
	Dormant,
	Telegraphing,
	Active
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrapTelegraphStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrapActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrapDeactivated);

/**
 *  주기적으로 움직이며 접촉 시 피해를 주는 동적 장애물(kinetic hazard) 베이스.
 *  Pivot 왕복(Oscillator) + 블레이드 접촉 피해(ContactDamage)를 조립한다.
 *
 *  활성화 흐름: Dormant → (플레이어 근접/외부 트리거) → Telegraphing(예고) → Active.
 *  텔레그래프 동안 OnTelegraphStarted를 브로드캐스트하므로 BP에서 떨림/빛/사운드 연출을 붙인다.
 *
 *  변종(BP):
 *   - 바닥 블레이드: Oscillator = Translation (소켓 안에서 슬라이드)
 *   - 천장 도끼:     Oscillator = Rotation (스윙), 블레이드를 Pivot 아래로 오프셋
 */
UCLASS()
class ABYSSGEN_API ATrapHazard : public AActor, public ISpawnable
{
	GENERATED_BODY()

public:
	ATrapHazard();

	virtual float GetGroundOffset_Implementation() override;

	/** 외부(압력판/상자 등)에서 트랩을 작동시킴: 텔레그래프 → 작동. */
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void TriggerActivation();

	/** 휴면 상태로 되돌림(재예고 가능). */
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void ResetToDormant();

	UFUNCTION(BlueprintPure, Category = "Trap")
	ETrapState GetTrapState() const { return TrapState; }

	/** 예고 시작 — BP에서 떨림/빛/사운드 연출 바인딩 */
	UPROPERTY(BlueprintAssignable, Category = "Trap")
	FOnTrapTelegraphStarted OnTelegraphStarted;

	/** 작동(위험) 시작 */
	UPROPERTY(BlueprintAssignable, Category = "Trap")
	FOnTrapActivated OnActivated;

	/** 휴면 복귀 */
	UPROPERTY(BlueprintAssignable, Category = "Trap")
	FOnTrapDeactivated OnDeactivated;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<USceneComponent> SceneRoot;

	/** 바닥/천장에 고정되는 마운트(소켓/프레임). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<UStaticMeshComponent> MountMesh;

	/** 왕복 운동의 피벗 (Oscillator 대상). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<USceneComponent> Pivot;

	/** 실제 피해를 주는 블레이드 (ContactDamage 감시 대상). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<UStaticMeshComponent> BladeMesh;

	/** 플레이어 근접 시 작동시키는 활성화 트리거 볼륨. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<USphereComponent> ActivationTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<UOscillatingMovementComponent> Oscillator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<UContactDamageComponent> ContactDamage;

	/** true면 처음부터 항상 작동(휴면/예고 생략). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Activation")
	bool bStartActive = false;

	/** 예고 지속 시간(초). 0이면 즉시 작동. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Activation", meta = (ClampMin = "0.0", Units = "s"))
	float TelegraphDuration = 0.6f;

	/** 플레이어가 트리거를 벗어나면 다시 휴면으로 복귀. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Activation")
	bool bRetriggerable = false;

	/** 바닥에서 띄울 거리(ISpawnable). 천장 도끼는 수동 배치이므로 보통 0. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap", meta = (ClampMin = "0.0", Units = "cm"))
	float GroundOffset = 0.0f;

	UFUNCTION()
	void HandleActivationBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleActivationEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void EnterDormant(bool bBroadcast);
	void EnterTelegraph();
	void EnterActive();

	ETrapState TrapState = ETrapState::Dormant;

	FTimerHandle TelegraphTimer;
};
