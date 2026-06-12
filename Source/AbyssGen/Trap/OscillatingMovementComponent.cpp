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

	// 이동(왕복) 채널 — 사인 곡선이라 양 끝에서 감속하는 부드러운 ease 이동
	if (MotionType == EOscillationMotionType::Translation)
	{
		TargetComponent->SetRelativeLocation(InitialRelativeLocation + NormalizedAxis * (Amplitude * Wave));
	}

	// 회전 채널 — 왕복 회전(MotionType==Rotation)과 등속 스핀(bEnableSpin)을 합성
	if (MotionType == EOscillationMotionType::Rotation || bEnableSpin)
	{
		FQuat FinalRotation = InitialRelativeRotation;

		if (MotionType == EOscillationMotionType::Rotation)
		{
			FinalRotation = FinalRotation * FQuat(NormalizedAxis, FMath::DegreesToRadians(Amplitude * Wave));
		}

		if (bEnableSpin)
		{
			const float SpinAngle = FMath::DegreesToRadians(SpinSpeed * ElapsedTime);
			FinalRotation = FinalRotation * FQuat(SpinAxis.GetSafeNormal(), SpinAngle);
		}

		TargetComponent->SetRelativeRotation(FinalRotation);
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
