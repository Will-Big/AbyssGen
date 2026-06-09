#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Chest.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AChest;

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

private:
	bool bIsOpen = false;
};
