// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDG/RoomBase.h"
#include "Interactable.h"
#include "RB_DungeonElevatorRoom.generated.h"


UCLASS()
class ABYSSGEN_API ARB_DungeonElevatorRoom : public ARoomBase, public IInteractable
{
	GENERATED_BODY()

protected:
	ARB_DungeonElevatorRoom();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Elevator;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* ElevatorCollision;

	FVector StartLocation;

	FVector EndLocation;

	bool bShouldMove;

	bool bIsUp;

	/** 엘리베이터 이동 속도 (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elevator", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float MoveSpeed = 600.0f;

	/** 상승 시 올라가는 높이 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elevator", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float ClimbHeight = 1900.0f;

	void GoUp();

	void GoDown();

	/** 현재 상태에 따라 GoUp/GoDown 토글 */
	void ToggleElevator();

public:
	/** IInteractable */
	virtual void OnInteractorEnter_Implementation(AActor* Interactor) override;
	virtual void OnInteractorExit_Implementation(AActor* Interactor) override;
};
