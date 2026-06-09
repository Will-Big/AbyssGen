#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterTargetingComponent.generated.h"

class ACharacter;

UCLASS(ClassGroup = (Monster), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UMonsterTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterTargetingComponent();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	AActor* FindPlayerTarget(int32 PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Targeting")
	AActor* GetTargetActor() const { return TargetActor; }

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool HasValidTarget() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	float GetDistanceToTarget() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool HasLineOfSightToTarget() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsTargetInAttackRange() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void FaceTarget();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ClampMin = "0.0", Units = "cm"))
	float DetectionRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackRange = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	bool bRequireLineOfSight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	TEnumAsByte<ECollisionChannel> LineOfSightTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AActor> TargetActor;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	bool HasLineOfSightToActor(const AActor* OtherActor) const;
};
