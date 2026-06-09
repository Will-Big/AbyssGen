// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TimerHandle.h"
#include "SpawnIntroComponent.generated.h"

/** 스폰 인트로가 끝났을 때 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnIntroFinishedSignature);

class ACharacter;
class UMaterialInstanceDynamic;

/**
 *  스폰 연출(정지 + 디졸브 머티리얼라이즈)을 담당하는 재사용 컴포넌트.
 *  Play() 시 소유 캐릭터의 이동을 멈추고, Duration 동안 머티리얼의 진행도 파라미터를
 *  StartValue→EndValue로 보간한 뒤 이동을 복원한다. 자체 틱을 사용한다.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API USpawnIntroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpawnIntroComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 소유 캐릭터를 정지시키고 Duration 동안 디졸브 연출을 재생한다. */
	UFUNCTION(BlueprintCallable, Category = "Spawn Intro")
	void Play(float InDuration);

	/** 정상 종료: 진행도를 끝까지 채우고 이동을 복원한 뒤 통지한다. */
	UFUNCTION(BlueprintCallable, Category = "Spawn Intro")
	void Finish();

	/** 중단: 타이머/틱만 정리한다(이동 복원·통지 없음. 소유자 사망 등에 사용). */
	UFUNCTION(BlueprintCallable, Category = "Spawn Intro")
	void Cancel();

	UFUNCTION(BlueprintPure, Category = "Spawn Intro")
	bool IsPlaying() const { return bIsPlaying; }

	UPROPERTY(BlueprintAssignable, Category = "Spawn Intro")
	FOnSpawnIntroFinishedSignature OnFinished;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Intro")
	bool bUseMaterialParameter = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Intro")
	FName ProgressParameterName = FName("SpawnProgress");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Intro")
	float StartValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Intro")
	float EndValue = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MaterialInstances;

	bool bIsPlaying = false;

	float Duration = 0.0f;

	float ElapsedTime = 0.0f;

	FTimerHandle FinishTimer;

	void PrepareMaterials();

	void SetProgress(float Progress);

	void FreezeOwner();

	void UnfreezeOwner();
};
