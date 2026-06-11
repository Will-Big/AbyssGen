#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/TimerHandle.h"
#include "PlayerMeleeAttackComponent.generated.h"

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UPlayerMeleeAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerMeleeAttackComponent();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void CancelAttack();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ExecuteAttackTrace();

	UFUNCTION(BlueprintPure, Category = "Attack")
	bool IsAttacking() const { return bIsAttacking; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackDamage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackTraceDistance = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackTraceRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0", Units = "s"))
	float AttackFallbackDuration = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bFaceControllerYawOnAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Debug")
	bool bDrawAttackDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Debug", meta = (ClampMin = "0.0", Units = "s"))
	float AttackDebugDrawTime = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	bool bIsAttacking = false;

	FOnMontageEnded AttackMontageEndedDelegate;

	FTimerHandle AttackFinishTimer;

	void PerformAttackTrace();

	void FinishAttack();

	void HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void FaceControllerYaw();
};
