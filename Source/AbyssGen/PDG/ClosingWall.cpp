#include "PDG/ClosingWall.h"

AClosingWall::AClosingWall()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ClosingWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall"));
	ClosingWall->SetupAttachment(Root);
}

void AClosingWall::BeginPlay()
{
	Super::BeginPlay();
	
}

void AClosingWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

