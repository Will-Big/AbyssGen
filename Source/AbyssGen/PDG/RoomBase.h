// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

class UBoxComponent;
class UArrowComponent;
class UCanvas;
class APlayerController;

UCLASS()
class ABYSSGEN_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomBase();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* GeometryFolder;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* OverlapFolder;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* ExitPointsFolder;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* FloorSpawnPoints;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShowDebugRoomName = true;
#endif

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITORONLY_DATA
	/** 화면 공간에 방 BP 이름을 그린다(벽에 가려지지 않고 PIE/Simulate 모두 표시). */
	void DrawRoomNameHUD(UCanvas* Canvas, APlayerController* PC);

	TArray<FDelegateHandle> DebugDrawHandles;
#endif

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Arrow;
};
