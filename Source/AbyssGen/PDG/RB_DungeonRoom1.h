// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDG/RoomBase.h"
#include "RB_DungeonRoom1.generated.h"

/**
 * 
 */
UCLASS()
class ABYSSGEN_API ARB_DungeonRoom1 : public ARoomBase
{
	GENERATED_BODY()

public:
	ARB_DungeonRoom1();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_3;
};
