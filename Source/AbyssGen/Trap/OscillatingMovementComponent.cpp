// Fill out your copyright notice in the Description page of Project Settings.

#include "Trap/OscillatingMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"

UOscillatingMovementComponent::UOscillatingMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UOscillatingMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetComponent && GetOwner())
	{
		TargetComponent = GetOwner()->GetRootComponent();
	}

	if (TargetComponent)
	{
		InitialRelativeLocation = TargetComponent->GetRelativeLocation();
		InitialRelativeRotation = TargetComponent->GetRelativeRotation().Quaternion();
	}
}

void UOscillatingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TargetComponent || !IsActive())
	{
		return;
	}

	ElapsedTime += DeltaTime;

	const float Phase = 2.0f * PI * (ElapsedTime / Period + PhaseOffset);
	const float Wave = FMath::Sin(Phase);
	const FVector NormalizedAxis = Axis.GetSafeNormal();

	if (MotionType == EOscillationMotionType::Translation)
	{
		TargetComponent->SetRelativeLocation(InitialRelativeLocation + NormalizedAxis * (Amplitude * Wave));
	}
	else
	{
		const FQuat Oscillation(NormalizedAxis, FMath::DegreesToRadians(Amplitude * Wave));
		TargetComponent->SetRelativeRotation(InitialRelativeRotation * Oscillation);
	}
}

void UOscillatingMovementComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	ElapsedTime = 0.0f;
}

void UOscillatingMovementComponent::Deactivate()
{
	Super::Deactivate();

	if (TargetComponent)
	{
		TargetComponent->SetRelativeLocation(InitialRelativeLocation);
		TargetComponent->SetRelativeRotation(InitialRelativeRotation);
	}
}
