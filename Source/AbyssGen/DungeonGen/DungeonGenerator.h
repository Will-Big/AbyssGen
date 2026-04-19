#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonTypes.h"
#include "DungeonGenerator.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class ABYSSGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon", meta = (ShowOnlyInnerProperties))
	FDungeonGeneratorConfig Config;

	UPROPERTY(EditAnywhere, Category = "Dungeon|Mesh")
	TObjectPtr<UStaticMesh> FloorMesh;

	UPROPERTY(EditAnywhere, Category = "Dungeon|Mesh")
	TObjectPtr<UStaticMesh> WallMesh;

	UFUNCTION(CallInEditor, Category = "Dungeon")
	void Generate();

	UFUNCTION(CallInEditor, Category = "Dungeon")
	void StepSeparation();

	UFUNCTION(CallInEditor, Category = "Dungeon")
	void ClearFloor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Dungeon|Debug")
	FDungeonGraph DungeonGraph;

	UPROPERTY(VisibleAnywhere, Category = "Dungeon|Mesh")
	TObjectPtr<UInstancedStaticMeshComponent> FloorISM;

	UPROPERTY(VisibleAnywhere, Category = "Dungeon|Mesh")
	TObjectPtr<UInstancedStaticMeshComponent> WallISM;
};
