// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/Chest.h"
#include "PDG/Coin.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

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

	SpawnCoinBurst();

	OnChestOpened.Broadcast(this, Interactor);
}

void AChest::SpawnCoinBurst()
{
	UWorld* World = GetWorld();
	if (!CoinClass || !World)
	{
		return;
	}

	const int32 Count = FMath::RandRange(FMath::Min(MinCoinCount, MaxCoinCount), FMath::Max(MinCoinCount, MaxCoinCount));
	if (Count <= 0)
	{
		return;
	}

	const FVector Origin = GetActorLocation() + FVector(0.0f, 0.0f, CoinSpawnHeight);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Jitter(
			FMath::FRandRange(-CoinSpawnJitter, CoinSpawnJitter),
			FMath::FRandRange(-CoinSpawnJitter, CoinSpawnJitter),
			0.0f);

		ACoin* Coin = World->SpawnActor<ACoin>(CoinClass, Origin + Jitter, FRotator::ZeroRotator, SpawnParams);
		if (!Coin)
		{
			continue;
		}

		// 위로 솟구치는 속도 + 무작위 수평 방향(원뿔 산포)
		const float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
		const float HorizontalSpeed = FMath::FRandRange(0.0f, CoinLaunchHorizontalSpeed);
		const float UpSpeed = CoinLaunchUpSpeed + FMath::FRandRange(-CoinLaunchUpVariance, CoinLaunchUpVariance);

		const FVector LaunchVelocity(
			FMath::Cos(Angle) * HorizontalSpeed,
			FMath::Sin(Angle) * HorizontalSpeed,
			UpSpeed);

		Coin->Launch(LaunchVelocity);
	}
}

bool AChest::IsOpen() const
{
	return bIsOpen;
}
