#include "Combat/AnimNotify_PlayerAttackTrace.h"
#include "Combat/PlayerMeleeAttackComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_PlayerAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (UPlayerMeleeAttackComponent* AttackComponent = Owner->FindComponentByClass<UPlayerMeleeAttackComponent>())
	{
		AttackComponent->ExecuteAttackTrace();
	}
}

FString UAnimNotify_PlayerAttackTrace::GetNotifyName_Implementation() const
{
	return FString("Player Attack Trace");
}
