// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RB_DungeonElevatorRoom.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ARB_DungeonElevatorRoom::ARB_DungeonElevatorRoom()
{
	PrimaryActorTick.bCanEverTick = true;

	Elevator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator"));
	ElevatorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorCollision"));

	Elevator->SetupAttachment(GeometryFolder);
	ElevatorCollision->SetupAttachment(Elevator);
}

void ARB_DungeonElevatorRoom::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = Elevator->GetRelativeLocation();
}

void ARB_DungeonElevatorRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldMove)
	{
		const FVector NewLocation = FMath::VInterpConstantTo(Elevator->GetRelativeLocation(), EndLocation, DeltaTime, MoveSpeed);
		Elevator->SetRelativeLocation(NewLocation);

		if (NewLocation.Equals(EndLocation, 1.0f))
		{
			Elevator->SetRelativeLocation(EndLocation);
			bShouldMove = false;
		}
	}
}

void ARB_DungeonElevatorRoom::ToggleElevator()
{
	if (bShouldMove)
	{
		return;
	}

	if (bIsUp)
	{
		GoDown();
	}
	else
	{
		GoUp();
	}
}

void ARB_DungeonElevatorRoom::GoUp()
{
	EndLocation = StartLocation + FVector(0.0f, 0.0f, 1900.0f);
	bShouldMove = true;
	bIsUp = true;
}

void ARB_DungeonElevatorRoom::GoDown()
{
	EndLocation = StartLocation;
	bShouldMove = true;
	bIsUp = false;
}

void ARB_DungeonElevatorRoom::OnInteractorEnter_Implementation(AActor* Interactor)
{
	ToggleElevator();
}

void ARB_DungeonElevatorRoom::OnInteractorExit_Implementation(AActor* Interactor)
{
}
