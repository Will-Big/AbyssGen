#include "Monster/MonsterHitReactionComponent.h"
#include "Combat/HealthComponent.h"
#include "Combat/MeleeAttackComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

UMonsterHitReactionComponent::UMonsterHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HitReactionMontageEndedDelegate.BindUObject(this, &UMonsterHitReactionComponent::HandleHitReactionMontageEnded);
}

void UMonsterHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	HealthComponent = OwnerCharacter ? OwnerCharacter->FindComponentByClass<UHealthComponent>() : nullptr;

	if (HealthComponent)
	{
		HealthComponent->OnDamageTaken.AddDynamic(this, &UMonsterHitReactionComponent::HandleDamageTaken);
	}
}

void UMonsterHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitReactionFallbackTimer);
		World->GetTimerManager().ClearTimer(RecoveryTimer);
	}

	Super::EndPlay(EndPlayReason);
}

bool UMonsterHitReactionComponent::PlayHitReaction()
{
	if (!OwnerCharacter || !HitReactionMontage || !OwnerCharacter->GetMesh())
	{
		return false;
	}

	if (HealthComponent && !HealthComponent->IsAlive())
	{
		return false;
	}

	UWorld* World = GetWorld();
	const float CurrentTime = World ? World->GetTimeSeconds() : 0.0f;
	if (CurrentTime - LastReactionTime < MinimumInterval)
	{
		return false;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}

	bIsReacting = true;
	LastReactionTime = CurrentTime;

	InterruptCurrentActions();

	if (bStopCurrentMontage)
	{
		AnimInstance->StopAllMontages(0.05f);
	}

	const float MontageLength = AnimInstance->Montage_Play(HitReactionMontage, 1.0f);
	if (MontageLength <= 0.0f)
	{
		FinishRecovery();
		return false;
	}

	AnimInstance->Montage_SetEndDelegate(HitReactionMontageEndedDelegate, HitReactionMontage);

	if (World)
	{
		const float ReactionDuration = FallbackReactionDuration > 0.0f ? FallbackReactionDuration : MontageLength;
		World->GetTimerManager().SetTimer(
			HitReactionFallbackTimer,
			this,
			&UMonsterHitReactionComponent::FinishReactionAnimation,
			ReactionDuration,
			false
		);
	}

	return true;
}

void UMonsterHitReactionComponent::HandleDamageTaken(float DamageAmount, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	PlayHitFeedback();
	PlayHitReaction();
}

void UMonsterHitReactionComponent::PlayHitFeedback()
{
	if (!OwnerCharacter)
	{
		return;
	}

	USceneComponent* AttachComponent = OwnerCharacter->GetMesh();
	const FVector FallbackLocation = OwnerCharacter->GetActorLocation() + HitVFXOffset;
	FVector FeedbackLocation = FallbackLocation;
	FRotator FeedbackRotation = OwnerCharacter->GetActorRotation();

	if (AttachComponent && HitVFXSocketName != NAME_None && AttachComponent->DoesSocketExist(HitVFXSocketName))
	{
		FeedbackLocation = AttachComponent->GetSocketLocation(HitVFXSocketName) + HitVFXOffset;
		FeedbackRotation = AttachComponent->GetSocketRotation(HitVFXSocketName);
	}

	if (HitVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			HitVFX,
			FeedbackLocation,
			FeedbackRotation
		);
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			FeedbackLocation,
			HitSoundVolume,
			HitSoundPitch
		);
	}
}

void UMonsterHitReactionComponent::FinishReactionAnimation()
{
	if (!bIsReacting)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitReactionFallbackTimer);
	}

	RestoreOwnerMovement();

	if (RecoveryDuration <= 0.0f)
	{
		FinishRecovery();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RecoveryTimer,
			this,
			&UMonsterHitReactionComponent::FinishRecovery,
			RecoveryDuration,
			false
		);
	}
}

void UMonsterHitReactionComponent::FinishRecovery()
{
	if (!bIsReacting)
	{
		return;
	}

	bIsReacting = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitReactionFallbackTimer);
		World->GetTimerManager().ClearTimer(RecoveryTimer);
	}

	RestoreOwnerMovement();
}

void UMonsterHitReactionComponent::HandleHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitReactionMontage)
	{
		FinishReactionAnimation();
	}
}

void UMonsterHitReactionComponent::InterruptCurrentActions()
{
	if (bCancelAttackOnHit)
	{
		if (UMeleeAttackComponent* MeleeAttackComponent = OwnerCharacter->FindComponentByClass<UMeleeAttackComponent>())
		{
			MeleeAttackComponent->CancelAttack(true);
		}
	}

	if (bStopMovementOnHit)
	{
		StopOwnerMovement();
	}
}

void UMonsterHitReactionComponent::StopOwnerMovement()
{
	if (!OwnerCharacter)
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();

		if (bDisableMovementDuringReaction)
		{
			MovementComponent->DisableMovement();
		}
	}
}

void UMonsterHitReactionComponent::RestoreOwnerMovement()
{
	if (!OwnerCharacter || !bDisableMovementDuringReaction)
	{
		return;
	}

	if (HealthComponent && !HealthComponent->IsAlive())
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
	}
}
