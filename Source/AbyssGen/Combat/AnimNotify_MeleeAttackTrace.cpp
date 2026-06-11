#include "Combat/AnimNotify_MeleeAttackTrace.h"
#include "Combat/MeleeAttackComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_MeleeAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	if (UMeleeAttackComponent* AttackComponent = Owner->FindComponentByClass<UMeleeAttackComponent>())
	{
		AttackComponent->ExecuteAttackTrace();
	}
}

FString UAnimNotify_MeleeAttackTrace::GetNotifyName_Implementation() const
{
	return FString("Melee Attack Trace");
}
