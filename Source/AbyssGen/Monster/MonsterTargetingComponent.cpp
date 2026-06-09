#include "Monster/MonsterTargetingComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UMonsterTargetingComponent::UMonsterTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

AActor* UMonsterTargetingComponent::FindPlayerTarget(int32 PlayerIndex)
{
	AActor* Player = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
	if (!Player)
	{
		SetTargetActor(nullptr);
		return nullptr;
	}

	SetTargetActor(Player);

	if (GetDistanceToTarget() > DetectionRange)
	{
		SetTargetActor(nullptr);
		return nullptr;
	}

	if (bRequireLineOfSight && !HasLineOfSightToTarget())
	{
		SetTargetActor(nullptr);
		return nullptr;
	}

	return TargetActor;
}

void UMonsterTargetingComponent::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

bool UMonsterTargetingComponent::HasValidTarget() const
{
	return IsValid(TargetActor);
}

float UMonsterTargetingComponent::GetDistanceToTarget() const
{
	if (!OwnerCharacter || !HasValidTarget())
	{
		return TNumericLimits<float>::Max();
	}

	float Distance = FVector::Dist2D(OwnerCharacter->GetActorLocation(), TargetActor->GetActorLocation());

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		if (const UCapsuleComponent* TargetCapsule = TargetPawn->FindComponentByClass<UCapsuleComponent>())
		{
			Distance -= TargetCapsule->GetScaledCapsuleRadius();
		}
	}

	if (const UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent())
	{
		Distance -= OwnerCapsule->GetScaledCapsuleRadius();
	}

	return FMath::Max(0.0f, Distance);
}

bool UMonsterTargetingComponent::HasLineOfSightToTarget() const
{
	return HasValidTarget() && HasLineOfSightToActor(TargetActor);
}

bool UMonsterTargetingComponent::IsTargetInAttackRange() const
{
	if (!HasValidTarget() || GetDistanceToTarget() > AttackRange)
	{
		return false;
	}

	return !bRequireLineOfSight || HasLineOfSightToTarget();
}

void UMonsterTargetingComponent::FaceTarget()
{
	if (!OwnerCharacter || !HasValidTarget())
	{
		return;
	}

	const FVector ToTarget = TargetActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
	const FVector Direction2D = ToTarget.GetSafeNormal2D();
	if (Direction2D.IsNearlyZero())
	{
		return;
	}

	OwnerCharacter->SetActorRotation(FRotator(0.0f, Direction2D.Rotation().Yaw, 0.0f));
}

bool UMonsterTargetingComponent::HasLineOfSightToActor(const AActor* OtherActor) const
{
	if (!OwnerCharacter || !OtherActor)
	{
		return false;
	}

	const UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
	const float OwnerHalfHeight = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector TraceStart = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, OwnerHalfHeight * 0.75f);

	FVector TraceEnd = OtherActor->GetActorLocation();
	if (const APawn* TargetPawn = Cast<APawn>(OtherActor))
	{
		if (const UCapsuleComponent* TargetCapsule = TargetPawn->FindComponentByClass<UCapsuleComponent>())
		{
			TraceEnd.Z += TargetCapsule->GetScaledCapsuleHalfHeight() * 0.5f;
		}
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MonsterTargetingLineOfSight), false);
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.AddIgnoredActor(OtherActor);

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
