// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDG/RoomBase.h"
#include "RB_DungeonElevatorRoom.generated.h"


UCLASS()
class ABYSSGEN_API ARB_DungeonElevatorRoom : public ARoomBase
{
	GENERATED_BODY()

protected:
	ARB_DungeonElevatorRoom();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_3;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Exit_Arrow_4;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Floor_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Floor_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Floor_3;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Floor_4;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_3;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_4;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_5;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_6;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_7;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SecondF_Wall_8;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Closing_Wall_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Closing_Wall_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Closing_Wall_3;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Elevator; 

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* ElevatorCollision;

	FVector StartLocation;

	FVector EndLocation;

	bool bShouldMove;

	bool bIsUp;

	/** 엘리베이터 이동 속도 (cm/s) */
	float MoveSpeed = 600.0f;

	void GoUp();

	void GoDown();

public:
	/** 캐릭터가 올라탔을 때 호출 — 현재 상태에 따라 GoUp/GoDown 토글 */
	void ToggleElevator();
};
