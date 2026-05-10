// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RoomBase.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

ARoomBase::ARoomBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	GeometryFolder = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryFolder"));
	OverlapFolder = CreateDefaultSubobject<USceneComponent>(TEXT("OverlapFolder"));
	ExitPointsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("ExitPointsFolder"));

	Cube_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_1"));
	Cube_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_2"));
	Cube_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_3"));
	Cube_4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_4"));
	Cube_5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_5"));
	Cube_6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_6"));
	Cube_7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_7"));
	Cube_8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube_8"));
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	DefaultSceneRoot->SetupAttachment(RootComponent);
	GeometryFolder->SetupAttachment(DefaultSceneRoot);
	OverlapFolder->SetupAttachment(DefaultSceneRoot);
	ExitPointsFolder->SetupAttachment(DefaultSceneRoot);

	Arrow->SetupAttachment(DefaultSceneRoot);
	Cube_1->SetupAttachment(GeometryFolder);
	Cube_2->SetupAttachment(GeometryFolder);
	Cube_3->SetupAttachment(GeometryFolder);
	Cube_4->SetupAttachment(GeometryFolder);
	Cube_5->SetupAttachment(GeometryFolder);
	Cube_6->SetupAttachment(GeometryFolder);
	Cube_7->SetupAttachment(GeometryFolder);
	Cube_8->SetupAttachment(GeometryFolder);
	Floor->SetupAttachment(GeometryFolder);
	BoxCollision->SetupAttachment(OverlapFolder);

	Arrow->bHiddenInGame = false;
}

void ARoomBase::BeginPlay()
{
	Super::BeginPlay();

}

void ARoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

