#include "Monster/MonsterDeathComponent.h"
#include "Combat/MeleeAttackComponent.h"
#include "Monster/SpawnIntroComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

UMonsterDeathComponent::UMonsterDeathComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMonsterDeathComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UMonsterDeathComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DeathRemovalTimer);
	}

	Super::EndPlay(EndPlayReason);
}

void UMonsterDeathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bDissolvePlaying || DeathDissolveDuration <= 0.0f)
	{
		return;
	}

	DissolveElapsedTime += DeltaTime;
	const float Alpha = FMath::Clamp(DissolveElapsedTime / DeathDissolveDuration, 0.0f, 1.0f);
	const float DissolveValue = FMath::Lerp(AliveDissolveValue, DeadDissolveValue, Alpha);
	SetDissolveValue(DissolveValue);

	if (Alpha >= 1.0f)
	{
		bDissolvePlaying = false;
		SetComponentTickEnabled(false);
	}
}

void UMonsterDeathComponent::HandleDeath(AActor* DamageCauser)
{
	if (bDeathStarted || !OwnerCharacter)
	{
		return;
	}

	bDeathStarted = true;

	if (bCancelAttackOnDeath)
	{
		if (UMeleeAttackComponent* MeleeAttackComponent = OwnerCharacter->FindComponentByClass<UMeleeAttackComponent>())
		{
			MeleeAttackComponent->CancelAttack();
		}
	}

	if (bCancelSpawnIntroOnDeath)
	{
		if (USpawnIntroComponent* SpawnIntroComponent = OwnerCharacter->FindComponentByClass<USpawnIntroComponent>())
		{
			SpawnIntroComponent->Cancel();
		}
	}

	if (bDisableCollisionOnDeath)
	{
		if (UCapsuleComponent* CapsuleComponent = OwnerCharacter->GetCapsuleComponent())
		{
			CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (bDisableMovementOnDeath)
	{
		if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
			MovementComponent->DisableMovement();
		}
	}

	StartDeathDissolve();

	if (!bDestroyOwnerAfterDelay)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		const float RemovalDelay = bUseDissolveOnDeath
			? FMath::Max(DeathRemovalTime, DeathDissolveDuration)
			: DeathRemovalTime;

		World->GetTimerManager().SetTimer(
			DeathRemovalTimer,
			this,
			&UMonsterDeathComponent::RemoveOwnerFromLevel,
			RemovalDelay,
			false
		);
	}
}

void UMonsterDeathComponent::StartDeathDissolve()
{
	if (!bUseDissolveOnDeath || !OwnerCharacter)
	{
		return;
	}

	PrepareDissolveMaterials();
	SetDissolveValue(AliveDissolveValue);

	if (DeathDissolveDuration <= 0.0f)
	{
		SetDissolveValue(DeadDissolveValue);
		return;
	}

	bDissolvePlaying = true;
	DissolveElapsedTime = 0.0f;
	SetComponentTickEnabled(true);
}

void UMonsterDeathComponent::PrepareDissolveMaterials()
{
	DissolveMaterialInstances.Reset();

	if (!OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	const int32 MaterialCount = Mesh->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
		if (DynamicMaterial)
		{
			DissolveMaterialInstances.Add(DynamicMaterial);
		}
	}
}

void UMonsterDeathComponent::SetDissolveValue(float Value)
{
	for (UMaterialInstanceDynamic* DynamicMaterial : DissolveMaterialInstances)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(DissolveParameterName, Value);
		}
	}
}

void UMonsterDeathComponent::RemoveOwnerFromLevel()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->Destroy();
	}
}
