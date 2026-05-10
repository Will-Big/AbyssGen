// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDG/RoomBase.h"
#include "RB_DungeonRoom4.generated.h"

/**
 * 
 */
UCLASS()
class ABYSSGEN_API ARB_DungeonRoom4 : public ARoomBase
{
	GENERATED_BODY()
	
public:
	ARB_DungeonRoom4();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Closing_Wall_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Closing_Wall_2;
};
