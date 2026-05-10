#include "DungeonStage_Hello.h"

#include "Core/DungeonContext.h"
#include "Core/DungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UDungeonStage_Hello::UDungeonStage_Hello()
{
	DisplayName = TEXT("Hello (Dummy)");
	Summary = TEXT("배관 검증용 더미. 가짜 방 N개를 가로로 나란히 추가하고 박스로 그린다.");
}

void UDungeonStage_Hello::Execute(FDungeonContext& Ctx)
{
	const int32 Count = FMath::Max(1, RoomCount);
	const float Spacing = SpacingCells;

	for (int32 i = 0; i < Count; ++i)
	{
		FDungeonRoom Room;
		Room.Center = FVector2D(i * Spacing, 0.f);
		Room.Size = RoomSize;
		Room.bIsMain = (i == Count / 2);
		Room.Type = (i == 0) ? EDungeonRoomType::Start
		                     : (i == Count - 1) ? EDungeonRoomType::Boss
		                                        : EDungeonRoomType::Normal;
		Ctx.Rooms.Add(Room);
	}
}

void UDungeonStage_Hello::DrawDebug(const FDungeonContext& Ctx) const
{
	UWorld* World = Ctx.World;
	if (!World) return;

	const float Cell = AbyssGen::Dungeon::CellSizeUU;
	for (const FDungeonRoom& Room : Ctx.Rooms)
	{
		const FVector Center(Room.Center.X * Cell, Room.Center.Y * Cell, 0.f);
		const FVector Extent(Room.Size.X * Cell * 0.5f, Room.Size.Y * Cell * 0.5f, Cell * 0.5f);
		const FColor Color = Room.bIsMain ? FColor::Yellow : DebugColor;
		DrawDebugBox(World, Center + FVector(0, 0, Extent.Z), Extent, Color,
			/*bPersistent*/ true, /*LifeTime*/ -1.f, /*DepthPriority*/ 0, /*Thickness*/ 2.f);
	}
}
