// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AbyssMonsterAIController.h"
#include "Monster/AbyssMonsterBase.h"
#include "Components/StateTreeAIComponent.h"

AAbyssMonsterAIController::AAbyssMonsterAIController()
{
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
	check(StateTreeAI);

	bStartAILogicOnPossess = false;
	bAttachToPawn = true;

	StateTreeAI->SetStartLogicAutomatically(false);
}

void AAbyssMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAbyssMonsterBase* Monster = Cast<AAbyssMonsterBase>(InPawn);
	if (!Monster)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s possessed a non-AbyssMonster pawn: %s"), *GetName(), *GetNameSafe(InPawn));
		return;
	}

	UStateTree* MonsterStateTree = Monster->GetMonsterStateTree();
	if (!MonsterStateTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no MonsterStateTree set."), *GetNameSafe(Monster));
		return;
	}

	StateTreeAI->StopLogic(TEXT("Setting monster StateTree"));
	StateTreeAI->SetStateTree(MonsterStateTree);
	StateTreeAI->StartLogic();
}

void AAbyssMonsterAIController::OnUnPossess()
{
	if (StateTreeAI)
	{
		StateTreeAI->StopLogic(TEXT("Monster unpossessed"));
	}

	Super::OnUnPossess();
}
