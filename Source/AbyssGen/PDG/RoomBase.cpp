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
	FloorSpawnPoints = CreateDefaultSubobject<USceneComponent>(TEXT("FloorSpawnPoints"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	DefaultSceneRoot->SetupAttachment(RootComponent);
	GeometryFolder->SetupAttachment(DefaultSceneRoot);
	OverlapFolder->SetupAttachment(DefaultSceneRoot);
	ExitPointsFolder->SetupAttachment(DefaultSceneRoot);
	FloorSpawnPoints->SetupAttachment(DefaultSceneRoot);
	Arrow->SetupAttachment(DefaultSceneRoot);

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

