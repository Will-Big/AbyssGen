#include "PDG/EscapePortal.h"

#include "AbyssGenGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "NiagaraComponent.h"

AEscapePortal::AEscapePortal()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(DefaultSceneRoot);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(DefaultSceneRoot);
	PortalEffect->SetAutoActivate(true);
	PortalEffect->SetVisibility(true, true);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(DefaultSceneRoot);
	InteractionCollision->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionCollision->SetGenerateOverlapEvents(true);
}

void AEscapePortal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshPortalEffect();
}

void AEscapePortal::BeginPlay()
{
	Super::BeginPlay();
	RefreshPortalEffect();
}

void AEscapePortal::RefreshPortalEffect()
{
	if (!PortalEffect || !PortalEffect->GetAsset())
	{
		return;
	}

	PortalEffect->SetVisibility(true, true);
	PortalEffect->ReinitializeSystem();
	PortalEffect->Activate(true);
}

void AEscapePortal::OnInteractorEnter_Implementation(AActor* Interactor)
{
	if (bActivated)
	{
		return;
	}

	APawn* PlayerPawn = Cast<APawn>(Interactor);
	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	AAbyssGenGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AAbyssGenGameMode>() : nullptr;
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Escape portal %s could not find AAbyssGenGameMode"), *GetName());
		return;
	}

	bActivated = true;
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BP_OnPortalActivated(PlayerPawn);
	GameMode->CompleteRun(PlayerPawn);
}

void AEscapePortal::OnInteractorExit_Implementation(AActor* Interactor)
{
}
