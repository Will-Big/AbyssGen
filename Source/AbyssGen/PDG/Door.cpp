// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/Door.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	FacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingArrow"));

	DefaultSceneRoot->SetupAttachment(RootComponent);
	Door->SetupAttachment(DefaultSceneRoot);
	BoxCollision->SetupAttachment(DefaultSceneRoot);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));
	BoxCollision->SetGenerateOverlapEvents(true);
	FacingArrow->SetupAttachment(DefaultSceneRoot);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldMove)
	{
		const FRotator NewRotation = FMath::RInterpTo(Door->GetRelativeRotation(), EndRotation, DeltaTime, RotateSpeed);
		Door->SetRelativeRotation(NewRotation);

		if (NewRotation.Equals(EndRotation, 1.0f))
		{
			Door->SetRelativeRotation(EndRotation);
			bShouldMove = false;
		}
	}
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = Door->GetRelativeRotation();
}

void ADoor::OnInteractorEnter_Implementation(AActor* Interactor)
{
	// FacingArrow의 forward 기준으로 캐릭터의 앞/뒤를 판정해 반대 방향으로 열림
	float OpenYaw = 90.0f;
	if (Interactor)
	{
		const FVector ToInteractor = Interactor->GetActorLocation() - FacingArrow->GetComponentLocation();
		if (FVector::DotProduct(ToInteractor, FacingArrow->GetForwardVector()) > 0.0f)
		{
			OpenYaw = -90.0f;
		}
	}

	EndRotation = StartRotation + FRotator(0.0f, OpenYaw, 0.0f);
	bShouldMove = true;
}

void ADoor::OnInteractorExit_Implementation(AActor* Interactor)
{
	EndRotation = StartRotation;
	bShouldMove = true;
}



