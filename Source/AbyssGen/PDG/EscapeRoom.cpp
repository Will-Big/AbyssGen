#include "PDG/EscapeRoom.h"

#include "Components/ChildActorComponent.h"
#include "PDG/EscapePortal.h"

AEscapeRoom::AEscapeRoom()
{
	PortalClass = AEscapePortal::StaticClass();

	PortalComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("PortalComponent"));
	PortalComponent->SetupAttachment(DefaultSceneRoot);
	PortalComponent->SetChildActorClass(PortalClass);
	PortalComponent->SetVisibility(true, true);
	PortalComponent->SetHiddenInGame(false);
}

void AEscapeRoom::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!PortalClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Escape room %s has no PortalClass"), *GetName());
		return;
	}

	if (PortalComponent->GetChildActorClass() != PortalClass)
	{
		PortalComponent->SetChildActorClass(PortalClass);
	}

	// ChildActorTemplate은 자식 BP 수정 후 오래된 상태로 남을 수 있으므로
	// 방이 재구성될 때 최신 BP_EscapePortal CDO에서 다시 생성한다.
	PortalComponent->CreateChildActor();
	PortalComponent->SetVisibility(true, true);
	if (AEscapePortal* PortalActor = Cast<AEscapePortal>(PortalComponent->GetChildActor()))
	{
		PortalActor->SetActorHiddenInGame(false);
		PortalActor->SetIsTemporarilyHiddenInEditor(false);
		PortalActor->RefreshPortalEffect();
	}
}
