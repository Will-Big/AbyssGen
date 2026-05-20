// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/Door.h"
#include "Components/BoxComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));

	DefaultSceneRoot->SetupAttachment(RootComponent);
	Door->SetupAttachment(DefaultSceneRoot);
	BoxCollision->SetupAttachment(DefaultSceneRoot);

}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldMove)
	{
		CurrentLocation = Door->GetRelativeLocation();
		Door->SetRelativeLocation(FMath::VInterpTo(CurrentLocation, EndLocation, DeltaTime, MoveSpeed));

		if (FVector::Distance(CurrentLocation, EndLocation) < 1.0f)
		{
			bShouldMove = false;
			Door->SetRelativeLocation(EndLocation);
		}
	}
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = Door->GetRelativeLocation();
}

void ADoor::OpenDoor()
{
	EndLocation = StartLocation + FVector(0.0f, 0.0f, -395.0f);
	bShouldMove = true;
}

void ADoor::CloseDoor()
{
	EndLocation = StartLocation;
	bShouldMove = true;
}



