#pragma once

#include "CoreMinimal.h"
#include "PDG/RoomBase.h"
#include "Templates/SubclassOf.h"
#include "SpecialTreasureRoom.generated.h"

class AChest;
class UNiagaraSystem;
class USpawnPointComponent;

UCLASS()
class ABYSSGEN_API ASpecialTreasureRoom : public ARoomBase
{
	GENERATED_BODY()

public:
	ASpecialTreasureRoom();

	UFUNCTION(BlueprintCallable, Category = "Special Room|Ambush")
	void TriggerAmbush(AActor* InstigatorActor = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Special Room|Ambush")
	void RegisterChest(AChest* Chest);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush")
	FName AmbushSpawnTag = FName("Enemy.Ambush");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush")
	TArray<TSubclassOf<AActor>> AmbushMonsterClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush")
	TObjectPtr<UNiagaraSystem> AmbushSpawnVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush")
	FVector AmbushSpawnVFXOffset = FVector(0.0f, 0.0f, 50.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush", meta = (ClampMin = "0.0", Units = "s"))
	float AmbushSpawnDelay = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush")
	bool bSnapAmbushSpawnsToFloor = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush", meta = (ClampMin = "0.0", Units = "cm"))
	float AmbushGroundTraceHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Room|Ambush", meta = (ClampMin = "0.0", Units = "cm"))
	float AmbushGroundTraceDepth = 2000.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Special Room|Ambush")
	TArray<TObjectPtr<USpawnPointComponent>> AmbushSpawnPoints;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Special Room|Ambush")
	TObjectPtr<AChest> BoundChest;

	UFUNCTION()
	void HandleChestOpened(AChest* Chest, AActor* Interactor);

private:
	bool bAmbushTriggered = false;

	void CacheAmbushSpawnPoints();
	void BindChestOpenEvent();
	void SpawnAmbushMonsterAtPoint(const USpawnPointComponent* Point);
	void QueueAmbushMonsterAtPoint(const USpawnPointComponent* Point);
	void SpawnAmbushVFX(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	FVector ResolveSpawnLocation(const USpawnPointComponent* Point, TSubclassOf<AActor> ActorClass) const;
};
