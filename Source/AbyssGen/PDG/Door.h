// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Door.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class ABYSSGEN_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADoor();

	virtual void Tick(float DeltaTime) override;

	/** IInteractable */
	virtual void OnInteractorEnter_Implementation(AActor* Interactor) override;
	virtual void OnInteractorExit_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxCollision;

	/** 캐릭터 앞/뒤 판정 기준 — forward가 통로를 관통하는 방향으로 배치 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* FacingArrow;

	FRotator StartRotation;

	FRotator EndRotation;

	bool bShouldMove;

	/** 회전 보간 속도 */
	float RotateSpeed = 4.0f;
};
