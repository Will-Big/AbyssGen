// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbyssGenGameMode.h"

#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

AAbyssGenGameMode::AAbyssGenGameMode()
{
}

bool AAbyssGenGameMode::CompleteRun(AActor* EscapingPlayer)
{
	if (bRunCompleted || !EscapingPlayer)
	{
		return false;
	}

	bRunCompleted = true;

	if (ACharacter* Character = Cast<ACharacter>(EscapingPlayer))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
			MovementComponent->DisableMovement();
		}

		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			Character->DisableInput(PlayerController);
		}
	}

	BP_OnRunCompleted(EscapingPlayer);
	return true;
}

void AAbyssGenGameMode::BP_OnRunCompleted_Implementation(AActor* EscapingPlayer)
{
	if (GEngine && !RunCompletedMessage.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			RunCompletedMessageDuration,
			FColor::Green,
			RunCompletedMessage.ToString(),
			true,
			FVector2D(2.5f, 2.5f));
	}
}
