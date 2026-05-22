// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/Coin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ACoin::ACoin()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(DefaultSceneRoot);
	CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(CoinMesh);
	PickupCollision->SetCollisionProfileName(TEXT("Trigger"));
	PickupCollision->SetGenerateOverlapEvents(true);
}

void ACoin::BeginPlay()
{
	Super::BeginPlay();

	MeshBaseLocation = CoinMesh->GetRelativeLocation();
}

void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// 회전 연출
	CoinMesh->AddRelativeRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));

	// 상하 부양 연출
	const float ZOffset = FMath::Sin(ElapsedTime * BobSpeed) * BobAmplitude;
	CoinMesh->SetRelativeLocation(MeshBaseLocation + FVector(0.0f, 0.0f, ZOffset));
}

void ACoin::OnInteractorEnter_Implementation(AActor* Interactor)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Coin acquired"));
	}

	Destroy();
}

void ACoin::OnInteractorExit_Implementation(AActor* Interactor)
{
}

float ACoin::GetGroundOffset_Implementation()
{
	return HoverHeight;
}
