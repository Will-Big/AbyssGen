// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RB_DungeonRoom2.h"
#include "Components/ArrowComponent.h"

ARB_DungeonRoom2::ARB_DungeonRoom2()
{
	Exit_Arrow_1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_1"));
	Exit_Arrow_2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_2"));
	Closing_Wall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_1"));

	Exit_Arrow_1->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_2->SetupAttachment(ExitPointsFolder);
	Closing_Wall_1->SetupAttachment(GeometryFolder);
}
