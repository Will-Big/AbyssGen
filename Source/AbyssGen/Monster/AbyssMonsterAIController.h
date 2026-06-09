// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AbyssMonsterAIController.generated.h"

/**
 * 
 */
class UStateTreeAIComponent;

UCLASS()
class ABYSSGEN_API AAbyssMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAbyssMonsterAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStateTreeAIComponent> StateTreeAI;
};
