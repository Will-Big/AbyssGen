// Fill out your copyright notice in the Description page of Project Settings.

#include "Trap/ContactDamageComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"

UContactDamageComponent::UContactDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UContactDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!DamageCollision && GetOwner())
	{
		DamageCollision = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	}
}

void UContactDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!DamageCollision || Damage <= 0.0f || !IsActive())
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	DamageCollision->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	if (OverlappingActors.Num() == 0)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	AActor* Owner = GetOwner();

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || Actor == Owner)
		{
			continue;
		}

		const float* LastHit = LastHitTimes.Find(Actor);
		if (LastHit && (Now - *LastHit) < ReHitInterval)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(Actor, Damage, nullptr, Owner, UDamageType::StaticClass());
		LastHitTimes.Add(Actor, Now);
	}
}

void UContactDamageComponent::Deactivate()
{
	Super::Deactivate();
	LastHitTimes.Reset();
}
