// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/SpawnIntroComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

USpawnIntroComponent::USpawnIntroComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USpawnIntroComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void USpawnIntroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FinishTimer);
	}

	Super::EndPlay(EndPlayReason);
}

void USpawnIntroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsPlaying || Duration <= 0.0f)
	{
		return;
	}

	ElapsedTime += DeltaTime;
	const float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
	const float Progress = FMath::Lerp(StartValue, EndValue, Alpha);
	SetProgress(Progress);
}

void USpawnIntroComponent::Play(float InDuration)
{
	if (!OwnerCharacter)
	{
		return;
	}

	bIsPlaying = true;
	Duration = InDuration;
	ElapsedTime = 0.0f;

	FreezeOwner();
	PrepareMaterials();
	SetProgress(StartValue);
	SetComponentTickEnabled(true);

	if (InDuration <= 0.0f)
	{
		Finish();
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(FinishTimer, this, &USpawnIntroComponent::Finish, InDuration, false);
}

void USpawnIntroComponent::Finish()
{
	if (!bIsPlaying)
	{
		return;
	}

	bIsPlaying = false;
	SetProgress(EndValue);
	SetComponentTickEnabled(false);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FinishTimer);
	}

	UnfreezeOwner();
	OnFinished.Broadcast();
}

void USpawnIntroComponent::Cancel()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FinishTimer);
	}

	bIsPlaying = false;
	SetComponentTickEnabled(false);
}

void USpawnIntroComponent::PrepareMaterials()
{
	if (!bUseMaterialParameter || !OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	MaterialInstances.Reset();

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	const int32 MaterialCount = Mesh->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
		if (DynamicMaterial)
		{
			MaterialInstances.Add(DynamicMaterial);
		}
	}
}

void USpawnIntroComponent::SetProgress(float Progress)
{
	if (!bUseMaterialParameter)
	{
		return;
	}

	for (UMaterialInstanceDynamic* DynamicMaterial : MaterialInstances)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(ProgressParameterName, Progress);
		}
	}
}

void USpawnIntroComponent::FreezeOwner()
{
	if (!OwnerCharacter)
	{
		return;
	}

	if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
}

void USpawnIntroComponent::UnfreezeOwner()
{
	if (!OwnerCharacter)
	{
		return;
	}

	if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
}
