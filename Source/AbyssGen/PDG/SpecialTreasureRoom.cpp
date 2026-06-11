#include "PDG/SpecialTreasureRoom.h"

#include "Components/ChildActorComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Monster/AbyssMonsterBase.h"
#include "PDG/Chest.h"
#include "PDG/SpawnPoint.h"

ASpecialTreasureRoom::ASpecialTreasureRoom()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpecialTreasureRoom::BeginPlay()
{
	Super::BeginPlay();

	CacheAmbushSpawnPoints();
	BindChestOpenEvent();
}

void ASpecialTreasureRoom::TriggerAmbush(AActor* InstigatorActor)
{
	if (bAmbushTriggered)
	{
		return;
	}

	bAmbushTriggered = true;

	for (const USpawnPointComponent* Point : AmbushSpawnPoints)
	{
		QueueAmbushMonsterAtPoint(Point);
	}
}

void ASpecialTreasureRoom::RegisterChest(AChest* Chest)
{
	if (!Chest)
	{
		return;
	}

	if (BoundChest)
	{
		BoundChest->OnChestOpened.RemoveDynamic(this, &ASpecialTreasureRoom::HandleChestOpened);
	}

	BoundChest = Chest;
	BoundChest->OnChestOpened.AddUniqueDynamic(this, &ASpecialTreasureRoom::HandleChestOpened);
}

void ASpecialTreasureRoom::HandleChestOpened(AChest* Chest, AActor* Interactor)
{
	TriggerAmbush(Interactor);
}

void ASpecialTreasureRoom::CacheAmbushSpawnPoints()
{
	AmbushSpawnPoints.Reset();

	TArray<USpawnPointComponent*> FoundPoints;
	GetComponents(FoundPoints);

	for (USpawnPointComponent* Point : FoundPoints)
	{
		if (!Point || Point->SpawnTag != AmbushSpawnTag)
		{
			continue;
		}

		AmbushSpawnPoints.Add(Point);
	}
}

void ASpecialTreasureRoom::BindChestOpenEvent()
{
	if (BoundChest)
	{
		RegisterChest(BoundChest);
		return;
	}

	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents(ChildActorComponents);

	for (UChildActorComponent* ChildActorComponent : ChildActorComponents)
	{
		if (!ChildActorComponent)
		{
			continue;
		}

		if (AChest* Chest = Cast<AChest>(ChildActorComponent->GetChildActor()))
		{
			RegisterChest(Chest);
			return;
		}
	}
}

void ASpecialTreasureRoom::SpawnAmbushMonsterAtPoint(const USpawnPointComponent* Point)
{
	if (!Point || AmbushMonsterClasses.Num() == 0)
	{
		return;
	}

	const int32 ClassIndex = FMath::RandRange(0, AmbushMonsterClasses.Num() - 1);
	const TSubclassOf<AActor> MonsterClass = AmbushMonsterClasses[ClassIndex];
	if (!MonsterClass)
	{
		return;
	}

	const FVector SpawnLocation = ResolveSpawnLocation(Point, MonsterClass);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(MonsterClass, SpawnLocation, Point->GetComponentRotation(), SpawnParams);
	if (AAbyssMonsterBase* Monster = Cast<AAbyssMonsterBase>(SpawnedActor))
	{
		Monster->BeginSpawnIntro(AmbushSpawnDelay);
	}
}

void ASpecialTreasureRoom::QueueAmbushMonsterAtPoint(const USpawnPointComponent* Point)
{
	if (!Point || AmbushMonsterClasses.Num() == 0)
	{
		return;
	}

	const int32 ClassIndex = FMath::RandRange(0, AmbushMonsterClasses.Num() - 1);
	const TSubclassOf<AActor> MonsterClass = AmbushMonsterClasses[ClassIndex];
	if (!MonsterClass)
	{
		return;
	}

	const FVector SpawnLocation = ResolveSpawnLocation(Point, MonsterClass);
	const FRotator SpawnRotation = Point->GetComponentRotation();

	SpawnAmbushVFX(SpawnLocation, SpawnRotation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		MonsterClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	if (AAbyssMonsterBase* Monster = Cast<AAbyssMonsterBase>(SpawnedActor))
	{
		Monster->BeginSpawnIntro(AmbushSpawnDelay);
	}
}

void ASpecialTreasureRoom::SpawnAmbushVFX(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!AmbushSpawnVFX)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		AmbushSpawnVFX,
		SpawnLocation + AmbushSpawnVFXOffset,
		SpawnRotation,
		FVector(1.0f),
		true,
		true
	);
}

FVector ASpecialTreasureRoom::ResolveSpawnLocation(const USpawnPointComponent* Point, TSubclassOf<AActor> ActorClass) const
{
	return USpawnPointComponent::ResolveGroundedSpawnLocation(
		GetWorld(),
		Point->GetComponentLocation(),
		ActorClass,
		bSnapAmbushSpawnsToFloor,
		AmbushGroundTraceHeight,
		AmbushGroundTraceDepth);
}
