// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AbyssGenGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AAbyssGenGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	AAbyssGenGameMode();

	/** 플레이어의 탈출을 한 번만 완료 처리하고 입력을 차단한다. */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool CompleteRun(AActor* EscapingPlayer);

	UFUNCTION(BlueprintPure, Category = "Game Flow")
	bool IsRunCompleted() const { return bRunCompleted; }

protected:
	/** 기본 완료 메시지를 표시한다. BP에서 재정의할 때 Parent 호출 후 추가 연출을 연결한다. */
	UFUNCTION(BlueprintNativeEvent, Category = "Game Flow", meta = (DisplayName = "On Run Completed"))
	void BP_OnRunCompleted(AActor* EscapingPlayer);
	virtual void BP_OnRunCompleted_Implementation(AActor* EscapingPlayer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Flow|Feedback")
	FText RunCompletedMessage = NSLOCTEXT("AbyssGen", "RunCompletedMessage", "ESCAPE COMPLETE");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Flow|Feedback", meta = (ClampMin = "0.0", Units = "s"))
	float RunCompletedMessageDuration = 8.0f;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Game Flow")
	bool bRunCompleted = false;
};



