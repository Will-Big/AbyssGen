// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/AbyssMonsterStateTreeUtility.h"
#include "Monster/AbyssMonsterBase.h"
#include "NavigationSystem.h"
#include "StateTreeAsyncExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"

EStateTreeRunStatus FStateTreeAbyssMonsterFindPlayerTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.TargetActor = nullptr;
	InstanceData.bHasTarget = false;

	if (!InstanceData.Monster)
	{
		return EStateTreeRunStatus::Running;
	}

	AActor* FoundTarget = InstanceData.Monster->FindPlayerTarget(InstanceData.PlayerIndex);
	if (!FoundTarget)
	{
		return EStateTreeRunStatus::Running;
	}

	if (InstanceData.Monster->GetDistanceToTarget() <= InstanceData.MaxRange)
	{
		InstanceData.TargetActor = FoundTarget;
		InstanceData.bHasTarget = true;
	}
	else
	{
		InstanceData.Monster->SetTargetActor(nullptr);
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterFindPlayerTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Find Player Target</b>");
}
#endif

EStateTreeRunStatus FStateTreeAbyssMonsterRandomWanderTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.Destination = FVector::ZeroVector;
	InstanceData.bFoundLocation = false;

	if (!InstanceData.Monster)
	{
		return EStateTreeRunStatus::Failed;
	}

	UWorld* World = InstanceData.Monster->GetWorld();
	if (!World)
	{
		return EStateTreeRunStatus::Failed;
	}

	UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavigationSystem)
	{
		return EStateTreeRunStatus::Failed;
	}

	FNavLocation RandomLocation;
	const bool bFoundLocation = NavigationSystem->GetRandomReachablePointInRadius(
		InstanceData.Monster->GetActorLocation(),
		InstanceData.Radius,
		RandomLocation
	);

	if (!bFoundLocation)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.Destination = RandomLocation.Location;
	InstanceData.bFoundLocation = true;

	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterRandomWanderTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Find Random Wander Location</b>");
}
#endif

EStateTreeRunStatus FStateTreeAbyssMonsterAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return EStateTreeRunStatus::Running;
	}

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Monster)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.Monster->OnAttackFinished.BindLambda(
		[WeakContext = Context.MakeWeakExecutionContext()]()
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	);

	if (!InstanceData.Monster->StartAttack())
	{
		InstanceData.Monster->OnAttackFinished.Unbind();
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeAbyssMonsterAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return;
	}

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.Monster)
	{
		InstanceData.Monster->OnAttackFinished.Unbind();
	}
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Monster Attack</b>");
}
#endif

EStateTreeRunStatus FStateTreeAbyssMonsterAttackPrepareDurationTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.Duration = 0.0f;

	if (!InstanceData.Monster)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.Duration = InstanceData.Monster->GetAttackPrepareDuration();
	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterAttackPrepareDurationTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Get Monster Attack Prepare Duration</b>");
}
#endif

EStateTreeRunStatus FStateTreeAbyssMonsterApproachAcceptanceRadiusTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.AcceptanceRadius = 0.0f;

	if (!InstanceData.Monster)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.AcceptanceRadius = InstanceData.Monster->GetApproachAcceptanceRadius();
	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterApproachAcceptanceRadiusTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Get Monster Approach Acceptance Radius</b>");
}
#endif

bool FStateTreeAbyssMonsterHasTargetCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const bool bHasTarget = InstanceData.Monster && InstanceData.Monster->HasValidTarget();
	return InstanceData.bMustHaveTarget ? bHasTarget : !bHasTarget;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterHasTargetCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Monster Has Target</b>");
}
#endif

bool FStateTreeAbyssMonsterTargetInAttackRangeCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const bool bIsInRange = InstanceData.Monster && InstanceData.Monster->IsTargetInAttackRange();
	return InstanceData.bMustBeInRange ? bIsInRange : !bIsInRange;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterTargetInAttackRangeCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Monster Target In Attack Range</b>");
}
#endif

bool FStateTreeAbyssMonsterAliveCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const bool bIsAlive = InstanceData.Monster && InstanceData.Monster->IsAlive();
	return InstanceData.bMustBeAlive ? bIsAlive : !bIsAlive;
}

#if WITH_EDITOR
FText FStateTreeAbyssMonsterAliveCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Monster Is Alive</b>");
}
#endif
