#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClosingWall.generated.h"

UCLASS()
class ABYSSGEN_API AClosingWall : public AActor
{
	GENERATED_BODY()
	
public:	
	AClosingWall();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ClosingWall;
};
