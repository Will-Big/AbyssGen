#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TimerHandle.h"
#include "MonsterDeathComponent.generated.h"

class ACharacter;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup = (Monster), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UMonsterDeathComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterDeathComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Death")
	void HandleDeath(AActor* DamageCauser);

	UFUNCTION(BlueprintPure, Category = "Death")
	bool IsDeathStarted() const { return bDeathStarted; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death", meta = (ClampMin = "0.0", Units = "s"))
	float DeathRemovalTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	bool bCancelAttackOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	bool bCancelSpawnIntroOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	bool bDisableCollisionOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	bool bDisableMovementOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	bool bDestroyOwnerAfterDelay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Dissolve")
	bool bUseDissolveOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Dissolve")
	FName DissolveParameterName = FName("SpawnProgress");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Dissolve")
	float AliveDissolveValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Dissolve")
	float DeadDissolveValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Dissolve", meta = (ClampMin = "0.0", Units = "s"))
	float DeathDissolveDuration = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DissolveMaterialInstances;

	bool bDeathStarted = false;

	bool bDissolvePlaying = false;

	float DissolveElapsedTime = 0.0f;

	FTimerHandle DeathRemovalTimer;

	void StartDeathDissolve();

	void PrepareDissolveMaterials();

	void SetDissolveValue(float Value);

	void RemoveOwnerFromLevel();
};
