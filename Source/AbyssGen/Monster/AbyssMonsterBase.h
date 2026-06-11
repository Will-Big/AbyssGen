// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Spawnable.h"
#include "Combat/Damageable.h"
#include "AbyssMonsterBase.generated.h"

DECLARE_DELEGATE(FOnAbyssMonsterAttackFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbyssMonsterDied);

class UStateTree;
class UHealthComponent;
class UMeleeAttackComponent;
class UMonsterTargetingComponent;
class USpawnIntroComponent;
class UMonsterDeathComponent;
class UMonsterHitReactionComponent;

UCLASS()
class ABYSSGEN_API AAbyssMonsterBase : public ACharacter, public ISpawnable, public IDamageable
{
	GENERATED_BODY()

public:
	AAbyssMonsterBase();

	UFUNCTION(BlueprintCallable, Category = "Monster|Target")
	AActor* FindPlayerTarget(int32 PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Monster|Target")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Monster|Target")
	AActor* GetTargetActor() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Target")
	bool HasValidTarget() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Target")
	float GetDistanceToTarget() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Target")
	bool HasLineOfSightToTarget() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Combat")
	bool IsTargetInAttackRange() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Combat")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Combat")
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Combat")
	float GetAttackPrepareDuration() const;

	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	void FaceTarget();

	UFUNCTION(BlueprintPure, Category = "Monster|AI")
	UStateTree* GetMonsterStateTree() const;

	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	bool StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Monster|Spawn")
	void BeginSpawnIntro(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Monster|Spawn")
	void FinishSpawnIntro();

	virtual float GetGroundOffset_Implementation() override;

	//~ Begin IDamageable interface
	virtual UHealthComponent* GetHealthComponent_Implementation() const override;
	//~ End IDamageable interface

	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	void Die();

	UPROPERTY(BlueprintAssignable, Category = "Monster|Events")
	FOnAbyssMonsterDied OnMonsterDied;

	FOnAbyssMonsterAttackFinished OnAttackFinished;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Stats")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Combat")
	TObjectPtr<UMeleeAttackComponent> MeleeAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Spawn")
	TObjectPtr<USpawnIntroComponent> SpawnIntroComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Targeting")
	TObjectPtr<UMonsterTargetingComponent> TargetingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Death")
	TObjectPtr<UMonsterDeathComponent> DeathComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Hit Reaction")
	TObjectPtr<UMonsterHitReactionComponent> HitReactionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	TObjectPtr<UStateTree> MonsterStateTree;

	/** HealthComponent의 OnDeath에 연결되는 사망 처리 */
	UFUNCTION()
	void HandleDeath(AActor* DamageCauser);

	/** MeleeAttackComponent의 OnAttackFinished를 단일 델리게이트로 전달 */
	UFUNCTION()
	void HandleAttackComponentFinished();

	bool IsSpawnIntroActive() const;

private:
};
