// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OscillatingMovementComponent.generated.h"

class USceneComponent;

UENUM(BlueprintType)
enum class EOscillationMotionType : uint8
{
	Translation,
	Rotation
};

/**
 *  대상 SceneComponent의 상대 트랜스폼을 사인 곡선으로 왕복시키는 재사용 컴포넌트.
 *  - Translation: 초기 위치 ± (Axis * Amplitude[cm])
 *  - Rotation:    초기 회전 ± (Axis 기준 Amplitude[deg])
 *  바닥 블레이드(이동)와 천장 도끼(회전)가 같은 컴포넌트를 공유한다.
 */
UCLASS(ClassGroup = (Trap), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UOscillatingMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOscillatingMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 왕복시킬 대상 컴포넌트를 지정한다(미지정 시 소유 액터 루트). */
	void SetTargetComponent(USceneComponent* InTarget) { TargetComponent = InTarget; }

	//~ 활성화 시 위상 초기화, 비활성화 시 초기 자세로 복귀
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oscillation")
	EOscillationMotionType MotionType = EOscillationMotionType::Translation;

	/** 이동 방향(Translation) 또는 회전 축(Rotation). 정규화되어 사용됨. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oscillation")
	FVector Axis = FVector(0.0f, 0.0f, 1.0f);

	/** 진폭: Translation=cm, Rotation=도(degree). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oscillation")
	float Amplitude = 100.0f;

	/** 1회 왕복 주기(초). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oscillation", meta = (ClampMin = "0.01", Units = "s"))
	float Period = 2.0f;

	/** 위상 오프셋(0~1). 여러 함정의 타이밍을 어긋나게 할 때 사용. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oscillation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PhaseOffset = 0.0f;

private:
	UPROPERTY()
	TObjectPtr<USceneComponent> TargetComponent;

	FVector InitialRelativeLocation = FVector::ZeroVector;

	FQuat InitialRelativeRotation = FQuat::Identity;

	float ElapsedTime = 0.0f;
};
