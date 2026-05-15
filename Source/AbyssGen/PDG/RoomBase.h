// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class ABYSSGEN_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* GeometryFolder;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* OverlapFolder;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* ExitPointsFolder;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_3;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_4;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_5;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_6;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_7;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cube_8;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Floor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Arrow;
};
