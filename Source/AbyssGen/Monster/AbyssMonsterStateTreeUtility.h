// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "StateTreeTaskBase.h"
#include "AbyssMonsterStateTreeUtility.generated.h"

class AAbyssMonsterBase;

USTRUCT()
struct FStateTreeAbyssMonsterInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAbyssMonsterBase> Monster;
};

USTRUCT()
struct FStateTreeAbyssMonsterAttackPrepareDurationInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAbyssMonsterBase> Monster;

	UPROPERTY(VisibleAnywhere, Category = "Output", meta = (Units = "s"))
	float Duration = 0.0f;
};

USTRUCT()
struct FStateTreeAbyssMonsterFindPlayerInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAbyssMonsterBase> Monster;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0"))
	int32 PlayerIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0", Units = "cm"))
	float MaxRange = 1500.0f;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	bool bHasTarget = false;
};

USTRUCT(meta = (DisplayName = "Find Player Target", Category = "Abyss Monster"))
struct FStateTreeAbyssMonsterFindPlayerTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterFindPlayerInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT()
struct FStateTreeAbyssMonsterRandomWanderInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAbyssMonsterBase> Monster;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0", Units = "cm"))
	float Radius = 600.0f;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	FVector Destination = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	bool bFoundLocation = false;
};

USTRUCT(meta = (DisplayName = "Find Random Wander Location", Category = "Abyss Monster"))
struct FStateTreeAbyssMonsterRandomWanderTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterRandomWanderInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT(meta = (DisplayName = "Monster Attack", Category = "Abyss Monster"))
struct FStateTreeAbyssMonsterAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT(meta = (DisplayName = "Get Monster Attack Prepare Duration", Category = "Abyss Monster"))
struct FStateTreeAbyssMonsterAttackPrepareDurationTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterAttackPrepareDurationInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT()
struct FStateTreeAbyssMonsterHasTargetConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	AAbyssMonsterBase* Monster = nullptr;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bMustHaveTarget = true;
};
STATETREE_POD_INSTANCEDATA(FStateTreeAbyssMonsterHasTargetConditionInstanceData);

USTRUCT(DisplayName = "Monster Has Target")
struct FStateTreeAbyssMonsterHasTargetCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterHasTargetConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT()
struct FStateTreeAbyssMonsterTargetRangeConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	AAbyssMonsterBase* Monster = nullptr;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bMustBeInRange = true;
};
STATETREE_POD_INSTANCEDATA(FStateTreeAbyssMonsterTargetRangeConditionInstanceData);

USTRUCT(DisplayName = "Monster Target In Attack Range")
struct FStateTreeAbyssMonsterTargetInAttackRangeCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterTargetRangeConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT()
struct FStateTreeAbyssMonsterAliveConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	AAbyssMonsterBase* Monster = nullptr;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bMustBeAlive = true;
};
STATETREE_POD_INSTANCEDATA(FStateTreeAbyssMonsterAliveConditionInstanceData);

USTRUCT(DisplayName = "Monster Is Alive")
struct FStateTreeAbyssMonsterAliveCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAbyssMonsterAliveConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
