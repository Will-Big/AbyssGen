// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AbyssMonsterBase.h"
#include "Monster/AbyssMonsterAIController.h"
#include "Combat/HealthComponent.h"
#include "Combat/MeleeAttackComponent.h"
#include "Monster/SpawnIntroComponent.h"
#include "Monster/MonsterTargetingComponent.h"
#include "Monster/MonsterDeathComponent.h"
#include "Monster/MonsterHitReactionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
AAbyssMonsterBase::AAbyssMonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AAbyssMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;

	GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;

	Tags.AddUnique(FName("Enemy"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetMaxHealth(3.0f);

	MeleeAttackComponent = CreateDefaultSubobject<UMeleeAttackComponent>(TEXT("MeleeAttackComponent"));

	SpawnIntroComponent = CreateDefaultSubobject<USpawnIntroComponent>(TEXT("SpawnIntroComponent"));

	TargetingComponent = CreateDefaultSubobject<UMonsterTargetingComponent>(TEXT("TargetingComponent"));

	DeathComponent = CreateDefaultSubobject<UMonsterDeathComponent>(TEXT("DeathComponent"));

	HitReactionComponent = CreateDefaultSubobject<UMonsterHitReactionComponent>(TEXT("HitReactionComponent"));

	MonsterPointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("MonsterPointLight"));
	MonsterPointLight->SetupAttachment(GetRootComponent());
	MonsterPointLight->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	MonsterPointLight->SetIntensity(250.0f);
	MonsterPointLight->SetAttenuationRadius(250.0f);
	MonsterPointLight->SetLightColor(FLinearColor(1.0f, 0.2f, 0.08f));
	MonsterPointLight->SetCastShadows(false);
}

// Called when the game starts or when spawned
void AAbyssMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AAbyssMonsterBase::HandleDeath);
	}

	if (MeleeAttackComponent)
	{
		MeleeAttackComponent->OnAttackFinished.AddDynamic(this, &AAbyssMonsterBase::HandleAttackComponentFinished);
	}
}

void AAbyssMonsterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

AActor* AAbyssMonsterBase::FindPlayerTarget(int32 PlayerIndex)
{
	if (IsSpawnIntroActive())
	{
		SetTargetActor(nullptr);
		return nullptr;
	}

	return TargetingComponent ? TargetingComponent->FindPlayerTarget(PlayerIndex) : nullptr;
}

void AAbyssMonsterBase::SetTargetActor(AActor* NewTarget)
{
	if (TargetingComponent)
	{
		TargetingComponent->SetTargetActor(NewTarget);
	}
}

AActor* AAbyssMonsterBase::GetTargetActor() const
{
	return TargetingComponent ? TargetingComponent->GetTargetActor() : nullptr;
}

bool AAbyssMonsterBase::HasValidTarget() const
{
	return TargetingComponent && TargetingComponent->HasValidTarget();
}

float AAbyssMonsterBase::GetDistanceToTarget() const
{
	return TargetingComponent ? TargetingComponent->GetDistanceToTarget() : TNumericLimits<float>::Max();
}

bool AAbyssMonsterBase::HasLineOfSightToTarget() const
{
	return TargetingComponent && TargetingComponent->HasLineOfSightToTarget();
}

bool AAbyssMonsterBase::IsTargetInAttackRange() const
{
	return TargetingComponent && TargetingComponent->IsTargetInAttackRange();
}

bool AAbyssMonsterBase::IsAlive() const
{
	return HealthComponent && HealthComponent->IsAlive();
}

bool AAbyssMonsterBase::IsAttacking() const
{
	return MeleeAttackComponent && MeleeAttackComponent->IsAttacking();
}

float AAbyssMonsterBase::GetAttackPrepareDuration() const
{
	return MeleeAttackComponent ? MeleeAttackComponent->GetAttackPrepareDuration() : 0.0f;
}

float AAbyssMonsterBase::GetApproachAcceptanceRadius() const
{
	return TargetingComponent ? TargetingComponent->GetApproachAcceptanceRadius() : 0.0f;
}

void AAbyssMonsterBase::FaceTarget()
{
	if (TargetingComponent)
	{
		TargetingComponent->FaceTarget();
	}
}

UStateTree* AAbyssMonsterBase::GetMonsterStateTree() const
{
	return MonsterStateTree;
}

bool AAbyssMonsterBase::StartAttack()
{
	if (!IsAlive() || IsSpawnIntroActive() || !MeleeAttackComponent)
	{
		return false;
	}

	if (HitReactionComponent && HitReactionComponent->IsReacting())
	{
		return false;
	}

	return MeleeAttackComponent->StartAttack(GetTargetActor());
}

void AAbyssMonsterBase::BeginSpawnIntro(float Duration)
{
	if (!IsAlive() || !SpawnIntroComponent)
	{
		return;
	}

	SetTargetActor(nullptr);

	if (MeleeAttackComponent)
	{
		MeleeAttackComponent->CancelAttack();
	}

	SpawnIntroComponent->Play(Duration);
}

void AAbyssMonsterBase::FinishSpawnIntro()
{
	if (SpawnIntroComponent)
	{
		SpawnIntroComponent->Finish();
	}
}

float AAbyssMonsterBase::GetGroundOffset_Implementation()
{
	return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

UHealthComponent* AAbyssMonsterBase::GetHealthComponent_Implementation() const
{
	return HealthComponent;
}

void AAbyssMonsterBase::Die()
{
	if (HealthComponent)
	{
		HealthComponent->Kill(nullptr);
	}
}

void AAbyssMonsterBase::HandleDeath(AActor* DamageCauser)
{
	if (DeathComponent)
	{
		DeathComponent->HandleDeath(DamageCauser);
	}

	OnMonsterDied.Broadcast();
}

void AAbyssMonsterBase::HandleAttackComponentFinished()
{
	OnAttackFinished.ExecuteIfBound();
}

bool AAbyssMonsterBase::IsSpawnIntroActive() const
{
	return SpawnIntroComponent && SpawnIntroComponent->IsPlaying();
}
