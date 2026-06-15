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

	// 물리 드롭 중에는 물리 엔진이 위치/회전을 제어하므로 부양 연출을 멈춘다.
	if (bPhysicsDrop)
	{
		return;
	}

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

void ACoin::Launch(const FVector& LaunchVelocity)
{
	if (bPhysicsDrop || !CoinMesh)
	{
		return;
	}

	bPhysicsDrop = true;

	// 바닥/서로는 막아 쌓이고, 플레이어와는 충돌하지 않는다(획득은 자식 PickupCollision 트리거가 담당).
	CoinMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CoinMesh->SetCollisionObjectType(ECC_PhysicsBody);
	CoinMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CoinMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CoinMesh->SetSimulatePhysics(true);

	// bVelChange=true: 질량을 무시하고 속도 변화로 적용(튜닝이 직관적).
	CoinMesh->AddImpulse(LaunchVelocity, NAME_None, true);
}
