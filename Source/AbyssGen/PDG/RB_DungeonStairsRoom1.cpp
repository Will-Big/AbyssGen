// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RB_DungeonStairsRoom1.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ARB_DungeonStairsRoom1::ARB_DungeonStairsRoom1()
{
	Exit_Arrow_1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_1"));
	Exit_Arrow_2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_2"));
	Exit_Arrow_3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_3"));
	Exit_Arrow_4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_4"));
	Floor_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor_2"));
	SecondF_Wall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_1"));
	SecondF_Wall_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_2"));
	SecondF_Wall_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_3"));
	SecondF_Wall_4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_4"));
	SecondF_Wall_5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_5"));
	SecondF_Wall_6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_6"));
	SecondF_Wall_7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_7"));
	SecondF_Wall_8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondF_Wall_8"));
	Closing_Wall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_1"));
	Closing_Wall_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_2"));
	Closing_Wall_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_3"));
	Stairs = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stairs"));

	Exit_Arrow_1->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_2->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_3->SetupAttachment(ExitPointsFolder);
	Exit_Arrow_4->SetupAttachment(ExitPointsFolder);
	Floor_2->SetupAttachment(GeometryFolder);
	SecondF_Wall_1->SetupAttachment(GeometryFolder);
	SecondF_Wall_2->SetupAttachment(GeometryFolder);
	SecondF_Wall_3->SetupAttachment(GeometryFolder);
	SecondF_Wall_4->SetupAttachment(GeometryFolder);
	SecondF_Wall_5->SetupAttachment(GeometryFolder);
	SecondF_Wall_6->SetupAttachment(GeometryFolder);
	SecondF_Wall_7->SetupAttachment(GeometryFolder);
	SecondF_Wall_8->SetupAttachment(GeometryFolder);
	Closing_Wall_1->SetupAttachment(GeometryFolder);
	Closing_Wall_2->SetupAttachment(GeometryFolder);
	Closing_Wall_3->SetupAttachment(GeometryFolder);
	Stairs->SetupAttachment(GeometryFolder);
}
