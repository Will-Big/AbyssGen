// Fill out your copyright notice in the Description page of Project Settings.

#include "Combat/MeleeAttackComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UMeleeAttackComponent::UMeleeAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AttackMontageEndedDelegate.BindUObject(this, &UMeleeAttackComponent::HandleAttackMontageEnded);
}

void UMeleeAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UMeleeAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackHitTimer);
		World->GetTimerManager().ClearTimer(AttackFinishTimer);
	}

	Super::EndPlay(EndPlayReason);
}

bool UMeleeAttackComponent::StartAttack(AActor* Target)
{
	if (bIsAttacking || !IsValid(Target) || !OwnerCharacter)
	{
		return false;
	}

	bIsAttacking = true;
	CurrentTarget = Target;

	FaceTarget();

	GetWorld()->GetTimerManager().SetTimer(
		AttackHitTimer,
		this,
		&UMeleeAttackComponent::PerformMeleeAttackTrace,
		AttackHitDelay,
		false
	);

	if (AttackMontage && OwnerCharacter->GetMesh())
	{
		if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(AttackMontage, 1.0f);
			if (MontageLength > 0.0f)
			{
				AnimInstance->Montage_SetEndDelegate(AttackMontageEndedDelegate, AttackMontage);
				return true;
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		AttackFinishTimer,
		this,
		&UMeleeAttackComponent::FinishAttack,
		AttackFallbackDuration,
		false
	);

	return true;
}

void UMeleeAttackComponent::CancelAttack()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackHitTimer);
		World->GetTimerManager().ClearTimer(AttackFinishTimer);
	}

	bIsAttacking = false;
	CurrentTarget = nullptr;
}

void UMeleeAttackComponent::PerformMeleeAttackTrace()
{
	if (!OwnerCharacter || !IsValid(CurrentTarget))
	{
		return;
	}

	if (bRequireLineOfSightOnHit && !HasLineOfSightToCurrentTarget())
	{
		return;
	}

	FaceTarget();

	TArray<FHitResult> Hits;
	const FVector TraceStart = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f);
	const FVector TraceEnd = TraceStart + OwnerCharacter->GetActorForwardVector() * AttackTraceDistance;

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	const FCollisionShape Shape = FCollisionShape::MakeSphere(AttackTraceRadius);
	const bool bHasAnyHit = GetWorld()->SweepMultiByObjectType(Hits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, QueryParams);

	bool bHitTarget = false;

	if (bHasAnyHit)
	{
		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == OwnerCharacter)
			{
				continue;
			}

			if (HitActor == CurrentTarget || HitActor->ActorHasTag(FName("Player")))
			{
				bHitTarget = true;
				UGameplayStatics::ApplyDamage(HitActor, AttackDamage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
				break;
			}
		}
	}

	if (bDrawAttackDebug)
	{
		const FColor DebugColor = bHitTarget ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, AttackDebugDrawTime, 0, 2.0f);
		DrawDebugSphere(GetWorld(), TraceEnd, AttackTraceRadius, 16, DebugColor, false, AttackDebugDrawTime);
	}
}

void UMeleeAttackComponent::FinishAttack()
{
	if (!bIsAttacking)
	{
		return;
	}

	bIsAttacking = false;
	CurrentTarget = nullptr;
	OnAttackFinished.Broadcast();
}

void UMeleeAttackComponent::HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		FinishAttack();
	}
}

void UMeleeAttackComponent::FaceTarget()
{
	if (!OwnerCharacter || !IsValid(CurrentTarget))
	{
		return;
	}

	const FVector ToTarget = CurrentTarget->GetActorLocation() - OwnerCharacter->GetActorLocation();
	const FVector Direction2D = ToTarget.GetSafeNormal2D();
	if (Direction2D.IsNearlyZero())
	{
		return;
	}

	OwnerCharacter->SetActorRotation(FRotator(0.0f, Direction2D.Rotation().Yaw, 0.0f));
}

bool UMeleeAttackComponent::HasLineOfSightToCurrentTarget() const
{
	if (!OwnerCharacter || !IsValid(CurrentTarget))
	{
		return false;
	}

	const UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
	const float OwnerHalfHeight = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector TraceStart = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, OwnerHalfHeight * 0.75f);

	FVector TraceEnd = CurrentTarget->GetActorLocation();
	if (const APawn* TargetPawn = Cast<APawn>(CurrentTarget))
	{
		if (const UCapsuleComponent* TargetCapsule = TargetPawn->FindComponentByClass<UCapsuleComponent>())
		{
			TraceEnd.Z += TargetCapsule->GetScaledCapsuleHalfHeight() * 0.5f;
		}
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MeleeAttackLineOfSight), false);
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.AddIgnoredActor(CurrentTarget);

	FHitResult Hit;
	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		LineOfSightTraceChannel,
		QueryParams
	);

	return !bBlocked;
}
