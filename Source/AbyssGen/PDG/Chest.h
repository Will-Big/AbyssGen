#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/SubclassOf.h"
#include "Interactable.h"
#include "Chest.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AChest;
class ACoin;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChestOpened, AChest*, Chest, AActor*, Interactor);

UCLASS()
class ABYSSGEN_API AChest : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AChest();

	virtual void Tick(float DeltaTime) override;

	virtual void OnInteractorEnter_Implementation(AActor* Interactor) override;
	virtual void OnInteractorExit_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "Chest")
	void Open(AActor* Interactor = nullptr);

	UFUNCTION(BlueprintPure, Category = "Chest")
	bool IsOpen() const;

	UPROPERTY(BlueprintAssignable, Category = "Chest|Events")
	FOnChestOpened OnChestOpened;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ChestBase;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* LidPivot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ChestLid;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractionCollision;

	UPROPERTY(EditAnywhere, Category = "Chest")
	float OpenAngle = -100.0f;

	UPROPERTY(EditAnywhere, Category = "Chest")
	float OpenSpeed = 5.0f;

	/** 열릴 때 솟아오를 코인 클래스(미지정 시 코인을 뿌리지 않음) */
	UPROPERTY(EditAnywhere, Category = "Chest|Reward")
	TSubclassOf<ACoin> CoinClass;

	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (ClampMin = "0"))
	int32 MinCoinCount = 4;

	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (ClampMin = "0"))
	int32 MaxCoinCount = 7;

	/** 코인 스폰 높이(액터 기준 Z, cm) */
	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (Units = "cm"))
	float CoinSpawnHeight = 60.0f;

	/** 스폰 지점 수평 산포 반경(cm) */
	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (ClampMin = "0.0", Units = "cm"))
	float CoinSpawnJitter = 15.0f;

	/** 위로 솟구치는 속도(cm/s) */
	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (ClampMin = "0.0"))
	float CoinLaunchUpSpeed = 450.0f;

	/** 위 속도 변동폭(±, cm/s) */
	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (ClampMin = "0.0"))
	float CoinLaunchUpVariance = 100.0f;

	/** 수평 산포 속도 최대치(cm/s) */
	UPROPERTY(EditAnywhere, Category = "Chest|Reward", meta = (ClampMin = "0.0"))
	float CoinLaunchHorizontalSpeed = 180.0f;

private:
	bool bIsOpen = false;

	/** 열릴 때 코인을 위로 발사해 바닥에 흩어지게 한다. */
	void SpawnCoinBurst();
};
