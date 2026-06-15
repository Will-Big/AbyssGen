// Fill out your copyright notice in the Description page of Project Settings.


#include "PDG/RoomBase.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"

ARoomBase::ARoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	GeometryFolder = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryFolder"));
	OverlapFolder = CreateDefaultSubobject<USceneComponent>(TEXT("OverlapFolder"));
	ExitPointsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("ExitPointsFolder"));
	FloorSpawnPoints = CreateDefaultSubobject<USceneComponent>(TEXT("FloorSpawnPoints"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	DefaultSceneRoot->SetupAttachment(RootComponent);
	GeometryFolder->SetupAttachment(DefaultSceneRoot);
	OverlapFolder->SetupAttachment(DefaultSceneRoot);
	ExitPointsFolder->SetupAttachment(DefaultSceneRoot);
	FloorSpawnPoints->SetupAttachment(DefaultSceneRoot);
	Arrow->SetupAttachment(DefaultSceneRoot);

	BoxCollision->SetupAttachment(OverlapFolder);

	Arrow->bHiddenInGame = false;
}

void ARoomBase::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITORONLY_DATA
	// 화면 공간 그리기를 PIE(Game) 뷰포트와 Simulate(Editor) 뷰포트 양쪽에 등록한다.
	const FDebugDrawDelegate Delegate = FDebugDrawDelegate::CreateUObject(this, &ARoomBase::DrawRoomNameHUD);
	DebugDrawHandles.Add(UDebugDrawService::Register(TEXT("Game"), Delegate));
	DebugDrawHandles.Add(UDebugDrawService::Register(TEXT("Editor"), Delegate));
#endif
}

void ARoomBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_EDITORONLY_DATA
	for (FDelegateHandle& Handle : DebugDrawHandles)
	{
		if (Handle.IsValid())
		{
			UDebugDrawService::Unregister(Handle);
		}
	}
	DebugDrawHandles.Reset();
#endif

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITORONLY_DATA
void ARoomBase::DrawRoomNameHUD(UCanvas* Canvas, APlayerController* PC)
{
	if (!bShowDebugRoomName || !Canvas)
	{
		return;
	}

	const FVector WorldLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);
	const FVector ScreenPosition = Canvas->Project(WorldLocation);
	if (ScreenPosition.Z <= 0.0f)
	{
		return; // 카메라 뒤쪽이면 그리지 않음
	}

	FString RoomName = GetClass()->GetName();
	RoomName.RemoveFromEnd(TEXT("_C"));

	UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
	if (!Font)
	{
		return;
	}

	FCanvasTextItem TextItem(FVector2D(ScreenPosition.X, ScreenPosition.Y), FText::FromString(RoomName), Font, FLinearColor(0.0f, 1.0f, 1.0f));
	TextItem.bCentreX = true;
	TextItem.bOutlined = true;
	TextItem.OutlineColor = FLinearColor::Black;
	Canvas->DrawItem(TextItem);
}
#endif

