// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/Chest.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	ChestBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestBase"));
	ChestBase->SetupAttachment(DefaultSceneRoot);

	LidPivot = CreateDefaultSubobject<USceneComponent>(TEXT("LidPivot"));
	LidPivot->SetupAttachment(ChestBase);

	ChestLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestLid"));
	ChestLid->SetupAttachment(LidPivot);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(DefaultSceneRoot);
	InteractionCollision->SetCollisionProfileName(TEXT("Trigger"));
	InteractionCollision->SetGenerateOverlapEvents(true);
}

void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpen)
	{
		const FRotator Current = LidPivot->GetRelativeRotation();
		if (!FMath::IsNearlyEqual(Current.Roll, OpenAngle, 0.5f))
		{
			const FRotator Target(Current.Pitch, Current.Yaw, OpenAngle);
			LidPivot->SetRelativeRotation(FMath::RInterpTo(Current, Target, DeltaTime, OpenSpeed));
		}
	}

}

void AChest::OnInteractorEnter_Implementation(AActor* Interactor)
{
	Open(Interactor);
}

void AChest::OnInteractorExit_Implementation(AActor* Interactor)
{
}

void AChest::Open(AActor* Interactor)
{
	if (bIsOpen)
	{
		return;
	}

	bIsOpen = true;
	OnChestOpened.Broadcast(this, Interactor);
}

bool AChest::IsOpen() const
{
	return bIsOpen;
}
