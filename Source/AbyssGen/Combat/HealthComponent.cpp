// Fill out your copyright notice in the Description page of Project Settings.

#include "Combat/HealthComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	ApplyDamage(Damage, DamageCauser);
}

float UHealthComponent::ApplyDamage(float DamageAmount, AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
	const float ActualDamage = OldHealth - CurrentHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDamageTaken.Broadcast(ActualDamage, DamageCauser);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast(DamageCauser);
	}

	return ActualDamage;
}

void UHealthComponent::Heal(float HealAmount)
{
	if (bIsDead || HealAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Kill(AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHealth = 0.0f;
	bIsDead = true;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDeath.Broadcast(DamageCauser);
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth, bool bRefill)
{
	MaxHealth = FMath::Max(1.0f, NewMaxHealth);

	if (bRefill)
	{
		CurrentHealth = MaxHealth;
		bIsDead = false;
	}
	else
	{
		CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UHealthComponent::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}
