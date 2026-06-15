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

	/** Move To가 공격 범위 안쪽까지 접근하도록 사용할 허용 반경. */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	float GetApproachAcceptanceRadius() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void FaceTarget();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ClampMin = "0.0", Units = "cm"))
	float DetectionRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackRange = 140.0f;

	/** 이동 종료 지점을 공격 범위보다 안쪽으로 당기는 여유 거리. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ClampMin = "0.0", Units = "cm"))
	float ApproachRangeBuffer = 20.0f;

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
