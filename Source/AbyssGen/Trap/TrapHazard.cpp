// Fill out your copyright notice in the Description page of Project Settings.

#include "Trap/TrapHazard.h"
#include "Trap/OscillatingMovementComponent.h"
#include "Trap/ContactDamageComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"

ATrapHazard::ATrapHazard()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MountMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MountMesh"));
	MountMesh->SetupAttachment(SceneRoot);
	MountMesh->SetCollisionProfileName(TEXT("BlockAll"));

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(SceneRoot);

	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeMesh"));
	BladeMesh->SetupAttachment(Pivot);
	BladeMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BladeMesh->SetGenerateOverlapEvents(true);

	ActivationTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(SceneRoot);
	ActivationTrigger->SetSphereRadius(300.0f);
	ActivationTrigger->SetCollisionProfileName(TEXT("Trigger"));
	ActivationTrigger->SetGenerateOverlapEvents(true);

	Oscillator = CreateDefaultSubobject<UOscillatingMovementComponent>(TEXT("Oscillator"));
	Oscillator->SetTargetComponent(Pivot);

	ContactDamage = CreateDefaultSubobject<UContactDamageComponent>(TEXT("ContactDamage"));
	ContactDamage->SetDamageCollision(BladeMesh);
}

void ATrapHazard::BeginPlay()
{
	Super::BeginPlay();

	if (ActivationTrigger)
	{
		ActivationTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrapHazard::HandleActivationBeginOverlap);
		ActivationTrigger->OnComponentEndOverlap.AddDynamic(this, &ATrapHazard::HandleActivationEndOverlap);
	}

	if (bStartActive)
	{
		EnterActive();
	}
	else
	{
		// 컴포넌트는 기본 활성이므로 휴면 시작 시 비활성화(브로드캐스트 없이).
		EnterDormant(false);
	}
}

float ATrapHazard::GetGroundOffset_Implementation()
{
	return GroundOffset;
}

void ATrapHazard::TriggerActivation()
{
	if (TrapState == ETrapState::Dormant)
	{
		EnterTelegraph();
	}
}

void ATrapHazard::ResetToDormant()
{
	if (TrapState != ETrapState::Dormant)
	{
		EnterDormant(true);
	}
}

void ATrapHazard::HandleActivationBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->ActorHasTag(FName("Player")))
	{
		return;
	}

	if (TrapState == ETrapState::Dormant)
	{
		EnterTelegraph();
	}
}

void ATrapHazard::HandleActivationEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bRetriggerable || !OtherActor || !OtherActor->ActorHasTag(FName("Player")))
	{
		return;
	}

	if (TrapState != ETrapState::Dormant)
	{
		EnterDormant(true);
	}
}

void ATrapHazard::EnterDormant(bool bBroadcast)
{
	TrapState = ETrapState::Dormant;

	GetWorldTimerManager().ClearTimer(TelegraphTimer);

	if (Oscillator)
	{
		Oscillator->Deactivate();
	}
	if (ContactDamage)
	{
		ContactDamage->Deactivate();
	}

	if (bBroadcast)
	{
		OnDeactivated.Broadcast();
	}
}

void ATrapHazard::EnterTelegraph()
{
	TrapState = ETrapState::Telegraphing;

	OnTelegraphStarted.Broadcast();

	if (TelegraphDuration <= 0.0f)
	{
		EnterActive();
		return;
	}

	GetWorldTimerManager().SetTimer(TelegraphTimer, this, &ATrapHazard::EnterActive, TelegraphDuration, false);
}

void ATrapHazard::EnterActive()
{
	TrapState = ETrapState::Active;

	if (Oscillator)
	{
		Oscillator->Activate(true);
	}
	if (ContactDamage)
	{
		ContactDamage->Activate(true);
	}

	OnActivated.Broadcast();
}
