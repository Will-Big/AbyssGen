#include "DungeonStage_Triangulate.h"

#include "Core/DungeonContext.h"
#include "Core/DungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// 외부 라이브러리. STL을 쓰므로 UE 매크로 충돌을 피하려면 가능한 한 cpp 안에서만 노출.
THIRD_PARTY_INCLUDES_START
#include "delaunator.hpp"
THIRD_PARTY_INCLUDES_END

UDungeonStage_Triangulate::UDungeonStage_Triangulate()
{
	DisplayName = TEXT("3. Triangulate");
	Summary = TEXT("메인 방 중심점을 Delaunay로 묶어 후보 간선을 만든다. (외부 라이브러리: delaunator-cpp)");
}

void UDungeonStage_Triangulate::Execute(FDungeonContext& Ctx)
{
	Ctx.DelaunayEdges.Reset();

	const int32 NMain = Ctx.MainRoomIndices.Num();
	if (NMain < 3)
	{
		// 점이 너무 적으면 단순 연결.
		for (int32 i = 0; i + 1 < NMain; ++i)
		{
			FDungeonEdge E;
			E.RoomA = Ctx.MainRoomIndices[i];
			E.RoomB = Ctx.MainRoomIndices[i + 1];
			E.Length = FVector2D::Distance(Ctx.Rooms[E.RoomA].Center, Ctx.Rooms[E.RoomB].Center);
			Ctx.DelaunayEdges.Add(E);
		}
		return;
	}

	// Main 인덱스 i (0..NMain-1) → Rooms 인덱스로의 매핑은 Ctx.MainRoomIndices.
	std::vector<double> Coords;
	Coords.reserve(NMain * 2);
	for (int32 mi = 0; mi < NMain; ++mi)
	{
		const FVector2D& C = Ctx.Rooms[Ctx.MainRoomIndices[mi]].Center;
		Coords.push_back(C.X);
		Coords.push_back(C.Y);
	}

	delaunator::Delaunator D(Coords);

	// 삼각형마다 3개 간선. 중복 제거를 위해 (작은인덱스, 큰인덱스) Set 사용.
	TSet<uint64> Seen;
	auto MakeKey = [](int32 A, int32 B) -> uint64
	{
		const uint32 Lo = (uint32)FMath::Min(A, B);
		const uint32 Hi = (uint32)FMath::Max(A, B);
		return ((uint64)Hi << 32) | Lo;
	};

	const auto& T = D.triangles;
	for (size_t k = 0; k + 2 < T.size(); k += 3)
	{
		const int32 A = (int32)T[k];
		const int32 B = (int32)T[k + 1];
		const int32 C = (int32)T[k + 2];

		const int32 Pairs[3][2] = { {A, B}, {B, C}, {C, A} };
		for (int32 p = 0; p < 3; ++p)
		{
			const int32 Ai = Pairs[p][0], Bi = Pairs[p][1];
			const uint64 Key = MakeKey(Ai, Bi);
			if (Seen.Contains(Key)) continue;
			Seen.Add(Key);

			FDungeonEdge E;
			E.RoomA = Ctx.MainRoomIndices[Ai];
			E.RoomB = Ctx.MainRoomIndices[Bi];
			E.Length = FVector2D::Distance(Ctx.Rooms[E.RoomA].Center, Ctx.Rooms[E.RoomB].Center);
			Ctx.DelaunayEdges.Add(E);
		}
	}
}

void UDungeonStage_Triangulate::DrawDebug(const FDungeonContext& Ctx) const
{
	UWorld* World = Ctx.World;
	if (!World) return;

	const float Cell = AbyssGen::Dungeon::CellSizeUU;
	const float Z = Cell * 1.1f; // 방 박스 약간 위에 그려서 가리지 않게
	for (const FDungeonEdge& E : Ctx.DelaunayEdges)
	{
		const FVector2D& A = Ctx.Rooms[E.RoomA].Center;
		const FVector2D& B = Ctx.Rooms[E.RoomB].Center;
		DrawDebugLine(World,
			FVector(A.X * Cell, A.Y * Cell, Z),
			FVector(B.X * Cell, B.Y * Cell, Z),
			FColor(160, 160, 160), true, -1.f, 0, 1.f);
	}
}
