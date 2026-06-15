#pragma once

#include "CoreMinimal.h"
#include "PDG/RoomBase.h"
#include "EscapeRoom.generated.h"

class AEscapePortal;
class UChildActorComponent;

/** 던전마다 정확히 하나 배치되는 탈출 목적 방. */
UCLASS()
class ABYSSGEN_API AEscapeRoom : public ARoomBase
{
	GENERATED_BODY()

public:
	AEscapeRoom();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Escape Room")
	TObjectPtr<UChildActorComponent> PortalComponent;

	/** BP_EscapePortal 같은 연출용 파생 클래스를 지정한다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Escape Room")
	TSubclassOf<AEscapePortal> PortalClass;
};
