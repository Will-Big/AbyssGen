// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RB_StarterRoom.h"
#include "Components/ArrowComponent.h"

ARB_StarterRoom::ARB_StarterRoom()
{
	Exit_Arrow_1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit_Arrow_1"));
	Closing_Wall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_1"));
	Closing_Wall_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_2"));
	Closing_Wall_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closing_Wall_3"));

	Exit_Arrow_1->SetupAttachment(ExitPointsFolder);
	Closing_Wall_1->SetupAttachment(GeometryFolder);
	Closing_Wall_2->SetupAttachment(GeometryFolder);
	Closing_Wall_3->SetupAttachment(GeometryFolder);
}
