// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Spawnable.h"
#include "Coin.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class ABYSSGEN_API ACoin : public AActor, public IInteractable, public ISpawnable
{
	GENERATED_BODY()

public:
	ACoin();

	virtual void Tick(float DeltaTime) override;

	/** IInteractable */
	virtual void OnInteractorEnter_Implementation(AActor* Interactor) override;
	virtual void OnInteractorExit_Implementation(AActor* Interactor) override;

	/** ISpawnable */
	virtual float GetGroundOffset_Implementation() override;

	/** 물리 드롭 모드로 전환하고 즉시 발사한다(상자 보상 연출 등). LaunchVelocity는 cm/s(질량 무시). */
	UFUNCTION(BlueprintCallable, Category = "Coin")
	void Launch(const FVector& LaunchVelocity);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* PickupCollision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* CoinMesh;

	/** 초당 Yaw 회전 속도(도) */
	UPROPERTY(EditAnywhere, Category = "Coin")
	float RotationSpeed = 90.0f;

	/** 상하 부양 진폭(cm) */
	UPROPERTY(EditAnywhere, Category = "Coin")
	float BobAmplitude = 10.0f;

	/** 상하 부양 속도 */
	UPROPERTY(EditAnywhere, Category = "Coin")
	float BobSpeed = 3.0f;

	/** 스폰 시 바닥에서 띄울 높이(cm) */
	UPROPERTY(EditAnywhere, Category = "Coin")
	float HoverHeight = 40.0f;

private:
	/** 부양 연출 기준이 되는 메시 상대 위치 */
	FVector MeshBaseLocation;

	float ElapsedTime = 0.0f;

	/** true면 물리 시뮬레이션 중(부양 연출 비활성) */
	bool bPhysicsDrop = false;
};
