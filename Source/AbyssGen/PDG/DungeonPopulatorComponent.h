// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnPoint.h"
#include "DungeonPopulatorComponent.generated.h"

class USpawnPointComponent;

/**
 *  던전 콘텐츠 배치 책임을 캡슐화하는 컴포넌트.
 *  생성된 방들에서 스폰 포인트를 수집(CollectFrom)하고,
 *  스폰 테이블/확률/Unique 규칙에 따라 액터를 배치(Populate)한다.
 *  방 토폴로지 생성(ADungeonGenerator)과 분리된 단일 책임을 가진다.
 */
UCLASS(ClassGroup = (PDG), meta = (BlueprintSpawnableComponent))
class ABYSSGEN_API UDungeonPopulatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDungeonPopulatorComponent();

	/** 방 액터에서 USpawnPointComponent를 모아 누적한다. */
	UFUNCTION(BlueprintCallable, Category = "Populator")
	void CollectFrom(AActor* Room);

	/** 수집된 스폰 포인트마다 규칙/확률을 적용해 액터를 스폰한다. */
	void Populate(FRandomStream& RandomStream);

protected:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TMap<ESpawnContentType, FSpawnTable> DefaultSpawnTables;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TMap<FName, FSpawnTable> TaggedSpawnTables;

private:
	UPROPERTY()
	TArray<TObjectPtr<USpawnPointComponent>> SpawnPoints;

	const FSpawnTable* ResolveSpawnTable(const USpawnPointComponent* Point) const;

	bool ShouldSpawnPoint(const USpawnPointComponent* Point, const FSpawnTable& Table, FRandomStream& RandomStream) const;

	void SpawnActorAtPoint(const USpawnPointComponent* Point, TSubclassOf<AActor> ActorClass);
};
