#include "Combat/PlayerMeleeAttackComponent.h"
#include "Combat/Damageable.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UPlayerMeleeAttackComponent::UPlayerMeleeAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AttackMontageEndedDelegate.BindUObject(this, &UPlayerMeleeAttackComponent::HandleAttackMontageEnded);
}

void UPlayerMeleeAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UPlayerMeleeAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelAttack();

	Super::EndPlay(EndPlayReason);
}

bool UPlayerMeleeAttackComponent::StartAttack()
{
	if (bIsAttacking || !OwnerCharacter)
	{
		return false;
	}

	bIsAttacking = true;

	if (bFaceControllerYawOnAttack)
	{
		FaceControllerYaw();
	}

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

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AttackFinishTimer,
			this,
			&UPlayerMeleeAttackComponent::FinishAttack,
			AttackFallbackDuration,
			false
		);
	}

	return true;
}

void UPlayerMeleeAttackComponent::CancelAttack()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackFinishTimer);
	}

	bIsAttacking = false;
}

void UPlayerMeleeAttackComponent::ExecuteAttackTrace()
{
	if (!bIsAttacking)
	{
		return;
	}

	PerformAttackTrace();
}

void UPlayerMeleeAttackComponent::PerformAttackTrace()
{
	if (!OwnerCharacter)
	{
		return;
	}

	const UCapsuleComponent* CapsuleComponent = OwnerCharacter->GetCapsuleComponent();
	const float HalfHeight = CapsuleComponent ? CapsuleComponent->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector TraceStart = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, HalfHeight * 0.5f);
	const FVector TraceEnd = TraceStart + OwnerCharacter->GetActorForwardVector() * AttackTraceDistance;

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerMeleeAttack), false);
	QueryParams.AddIgnoredActor(OwnerCharacter);

	TArray<FHitResult> Hits;
	const FCollisionShape Shape = FCollisionShape::MakeSphere(AttackTraceRadius);
	const bool bHasHit = GetWorld()->SweepMultiByObjectType(Hits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, QueryParams);

	TSet<TObjectPtr<AActor>> DamagedActors;
	bool bDamagedAnyActor = false;

	if (bHasHit)
	{
		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || DamagedActors.Contains(HitActor) || !HitActor->Implements<UDamageable>())
			{
				continue;
			}

			DamagedActors.Add(HitActor);
			bDamagedAnyActor = true;
			UGameplayStatics::ApplyDamage(HitActor, AttackDamage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
		}
	}

	if (bDrawAttackDebug)
	{
		const FColor DebugColor = bDamagedAnyActor ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, AttackDebugDrawTime, 0, 2.0f);
		DrawDebugSphere(GetWorld(), TraceEnd, AttackTraceRadius, 16, DebugColor, false, AttackDebugDrawTime);
	}
}

void UPlayerMeleeAttackComponent::FinishAttack()
{
	if (!bIsAttacking)
	{
		return;
	}

	bIsAttacking = false;
}

void UPlayerMeleeAttackComponent::HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		FinishAttack();
	}
}

void UPlayerMeleeAttackComponent::FaceControllerYaw()
{
	if (!OwnerCharacter || !OwnerCharacter->GetController())
	{
		return;
	}

	const FRotator ControlRotation = OwnerCharacter->GetController()->GetControlRotation();
	OwnerCharacter->SetActorRotation(FRotator(0.0f, ControlRotation.Yaw, 0.0f));
}
