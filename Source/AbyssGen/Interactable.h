// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 *  오버랩 기반 상호작용 계약
 *  상호작용 주체(캐릭터 등)와 오버랩될 때 호출된다.
 */
class IInteractable
{
	GENERATED_BODY()

public:
	/** 상호작용 주체가 범위에 진입했을 때 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnInteractorEnter(AActor* Interactor);

	/** 상호작용 주체가 범위를 벗어났을 때 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnInteractorExit(AActor* Interactor);
};
