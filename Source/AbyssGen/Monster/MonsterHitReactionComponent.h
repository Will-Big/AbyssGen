#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/TimerHandle.h"
#include "MonsterHitReactionComponent.generated.h"

class ACharacter;
class UHealthComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS(ClassGroup = (Monster), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UMonsterHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterHitReactionComponent();

	UFUNCTION(BlueprintCallable, Category = "Hit Reaction")
	bool PlayHitReaction();

	UFUNCTION(BlueprintPure, Category = "Hit Reaction")
	bool IsReacting() const { return bIsReacting; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool bStopCurrentMontage = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool bCancelAttackOnHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool bStopMovementOnHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool bDisableMovementDuringReaction = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction", meta = (ClampMin = "0.0", Units = "s"))
	float MinimumInterval = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction", meta = (ClampMin = "0.0", Units = "s"))
	float FallbackReactionDuration = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction", meta = (ClampMin = "0.0", Units = "s"))
	float RecoveryDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction|Feedback")
	TObjectPtr<UNiagaraSystem> HitVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction|Feedback")
	FName HitVFXSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction|Feedback")
	FVector HitVFXOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction|Feedback")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction|Feedback", meta = (ClampMin = "0.0"))
	float HitSoundVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction|Feedback", meta = (ClampMin = "0.01"))
	float HitSoundPitch = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UHealthComponent> HealthComponent;

	bool bIsReacting = false;

	float LastReactionTime = -TNumericLimits<float>::Max();

	FOnMontageEnded HitReactionMontageEndedDelegate;

	FTimerHandle HitReactionFallbackTimer;

	FTimerHandle RecoveryTimer;

	UFUNCTION()
	void HandleDamageTaken(float DamageAmount, AActor* DamageCauser);

	void PlayHitFeedback();

	void FinishReactionAnimation();

	void FinishRecovery();

	void HandleHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void InterruptCurrentActions();

	void StopOwnerMovement();

	void RestoreOwnerMovement();
};
