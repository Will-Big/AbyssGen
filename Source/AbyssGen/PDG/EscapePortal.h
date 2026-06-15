#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "EscapePortal.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UStaticMeshComponent;

/** 플레이어가 진입하면 현재 던전 플레이를 완료시키는 포탈. */
UCLASS()
class ABYSSGEN_API AEscapePortal : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AEscapePortal();

	/** Child Actor로 생성된 경우에도 Niagara 상태를 명시적으로 갱신한다. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Escape Portal")
	void RefreshPortalEffect();

	/** IInteractable */
	virtual void OnInteractorEnter_Implementation(AActor* Interactor) override;
	virtual void OnInteractorExit_Implementation(AActor* Interactor) override;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PortalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> PortalEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionCollision;

	/** 포탈 진입 연출을 Blueprint에서 시작할 수 있는 지점. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Escape Portal", meta = (DisplayName = "On Portal Activated"))
	void BP_OnPortalActivated(AActor* EscapingPlayer);

private:
	bool bActivated = false;
};
