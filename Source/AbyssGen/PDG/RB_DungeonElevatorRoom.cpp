// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RB_DungeonElevatorRoom.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ARB_DungeonElevatorRoom::ARB_DungeonElevatorRoom()
{
	PrimaryActorTick.bCanEverTick = true;

	Exit_Arrow_1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_1"));
	Exit_Arrow_2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_2"));
	Exit_Arrow_3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_3"));
	Exit_Arrow_4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_4"));
	Floor_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor_1"));
	Floor_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor_2"));
	Floor_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor_3"));
	Floor_4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor_4"));
	SecondF_Wall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_1"));
	SecondF_Wall_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_2"));
	SecondF_Wall_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_3"));
	SecondF_Wall_4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_4"));
	SecondF_Wall_5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_5"));
	SecondF_Wall_6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_6"));
	SecondF_Wall_7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_7"));
	SecondF_Wall_8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_8"));
	Closing_Wall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_1"));
	Closing_Wall_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_2"));
	Closing_Wall_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_3"));
	Elevator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator"));
	ElevatorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorCollision"));

	Exit_Arrow_1->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_2->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_3->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_4->SetupAttachment(ExitPointsFolder);
	Floor_1->SetupAttachment(GeometryFolder);
	Floor_2->SetupAttachment(GeometryFolder);
	Floor_3->SetupAttachment(GeometryFolder);
	Floor_4->SetupAttachment(GeometryFolder);
	SecondF_Wall_1->SetupAttachment(GeometryFolder);
	SecondF_Wall_2->SetupAttachment(GeometryFolder);
	SecondF_Wall_3->SetupAttachment(GeometryFolder);
	SecondF_Wall_4->SetupAttachment(GeometryFolder);
	SecondF_Wall_5->SetupAttachment(GeometryFolder);
	SecondF_Wall_6->SetupAttachment(GeometryFolder);
	SecondF_Wall_7->SetupAttachment(GeometryFolder);
	SecondF_Wall_8->SetupAttachment(GeometryFolder);
	Closing_Wall_1->SetupAttachment(GeometryFolder);
	Closing_Wall_2->SetupAttachment(GeometryFolder);
	Closing_Wall_3->SetupAttachment(GeometryFolder);
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
