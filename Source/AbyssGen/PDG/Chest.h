// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Chest.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class ABYSSGEN_API AChest : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AChest();

	virtual void Tick(float DeltaTime) override;

	/** IInteractable */
	virtual void OnInteractorEnter_Implementation(AActor* Interactor) override;
	virtual void OnInteractorExit_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ChestBase;

	/** 뚜껑 회전축 — 경첩 위치로 옮기고 이 컴포넌트를 Roll 회전시킨다 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* LidPivot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ChestLid;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractionCollision;

	/** 뚜껑이 완전히 열렸을 때 LidPivot의 Roll 각도(도) */
	UPROPERTY(EditAnywhere, Category = "Chest")
	float OpenAngle = -100.0f;

	/** 뚜껑 회전 보간 속도 */
	UPROPERTY(EditAnywhere, Category = "Chest")
	float OpenSpeed = 5.0f;

	/** 뚜껑을 여는 연출 시작 */
	void Open();

private:
	bool bIsOpen = false;
};
