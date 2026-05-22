// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Spawnable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class USpawnable : public UInterface
{
	GENERATED_BODY()
};

/**
 *  던전에 스폰되는 액터의 배치 규칙.
 *  DungeonGenerator가 스폰 위치를 보정할 때 참조한다.
 */
class ISpawnable
{
	GENERATED_BODY()

public:
	/** 바닥에서 띄울 거리(cm). 0이면 바닥에 붙는다. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spawn")
	float GetGroundOffset();
};
