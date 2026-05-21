// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Door.generated.h"

class UBoxComponent;

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

	FVector StartLocation;

	FVector EndLocation;

	FVector CurrentLocation;

	bool bShouldMove;

	float MoveSpeed = 10.0f;
};
