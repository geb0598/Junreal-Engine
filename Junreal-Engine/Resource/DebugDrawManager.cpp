#include "pch.h"
#include "Resource/DebugDrawManager.h"


FDebugDrawManager& FDebugDrawManager::GetInstance()
{
	static FDebugDrawManager Instance;
	return Instance;
}

void FDebugDrawManager::AddLine(const FVector& InStart, const FVector& InEnd, const FVector4& InColor)
{
	DebugLines.Add(FDebugLine{ InStart, InEnd, InColor });
}

void FDebugDrawManager::AddCircle(const FVector& InCenter, const FVector& InUpAxis, float InRadius, int InNumSegments, const FVector4& InColor)
{
	if (InNumSegments <= 2)
	{
		return;
	}
	FVector AxisU, AxisV;
	InUpAxis.FindBestAxisVectors(AxisU, AxisV);

	FVector LastVertex = InCenter + AxisU * InRadius;
	for (int i = 1;i <= InNumSegments; ++i)
	{
		float Angle = static_cast<float>(i) / static_cast<float>(InNumSegments) * TWO_PI;
		FVector Vertex = InCenter + (AxisU * cos(Angle) + AxisV * sin(Angle)) * InRadius;
		DebugLines.Add(FDebugLine{ LastVertex, Vertex, InColor });
		LastVertex = Vertex;
	}
}

void FDebugDrawManager::AddSphere(const FVector& InCenter, float InRadius, int InNumSegments, const FVector4& InColor)
{
	AddCircle(InCenter, FVector(1.0f, 0.0f, 0.0f), InRadius, InNumSegments, InColor); // XY 평면
	AddCircle(InCenter, FVector(0.0f, 1.0f, 0.0f), InRadius, InNumSegments, InColor); // XZ 평면
	AddCircle(InCenter, FVector(0.0f, 0.0f, 1.0f), InRadius, InNumSegments, InColor); // YZ 평면
}

void FDebugDrawManager::AddCone(const FVector& InApexPosition, const FVector& InDirection, float InHeight, float InAngleInDegrees, int InNumSegments, const FVector4& InColor)
{
	if (InNumSegments < 3 || InHeight <= 0.0f) return;

	// 원뿔의 기본 정보
	const FVector DirNorm = InDirection.GetSafeNormal();
	const FVector BaseCenter = InApexPosition + DirNorm * InHeight;
	const float AngleInRadians = DegreeToRadian(InAngleInDegrees);
	const float BaseRadius = tanf(AngleInRadians) * InHeight;

	// 밑면 원 그리기
	AddCircle(BaseCenter, DirNorm, BaseRadius, InNumSegments, InColor);

	// 꼭짓점에서 밑면으로 이어지는 선들 그리기
	FVector AxisU, AxisV;
	DirNorm.FindBestAxisVectors(AxisU, AxisV);

	// 일단, 4개의 옆면 선만 그림
	for (int i = 0; i < InNumSegments; ++i)
	{
		float Angle = static_cast<float>(i) / static_cast<float>(InNumSegments) * TWO_PI;
		FVector PointOnCircle = BaseCenter + (AxisU * cosf(Angle) + AxisV * sinf(Angle)) * BaseRadius;
		AddLine(InApexPosition, PointOnCircle, InColor);
	}
}



void FDebugDrawManager::AddBox(const FVector& InCenter, const FVector& InExtents, const FVector4& InColor)
{
	// 상자의 8개 꼭짓점 좌표
	FVector Vertices[8];
	Vertices[0] = InCenter + FVector(-InExtents.X, -InExtents.Y, -InExtents.Z); // ---
	Vertices[1] = InCenter + FVector(InExtents.X, -InExtents.Y, -InExtents.Z); // +--
	Vertices[2] = InCenter + FVector(InExtents.X, InExtents.Y, -InExtents.Z); // ++-
	Vertices[3] = InCenter + FVector(-InExtents.X, InExtents.Y, -InExtents.Z); // -+-
	Vertices[4] = InCenter + FVector(-InExtents.X, -InExtents.Y, InExtents.Z); // --+
	Vertices[5] = InCenter + FVector(InExtents.X, -InExtents.Y, InExtents.Z); // +-+
	Vertices[6] = InCenter + FVector(InExtents.X, InExtents.Y, InExtents.Z); // +++
	Vertices[7] = InCenter + FVector(-InExtents.X, InExtents.Y, InExtents.Z); // -++

	// === 12개의 모서리(Edge) ===
	// 아래쪽 면
	AddLine(Vertices[0], Vertices[1], InColor);
	AddLine(Vertices[1], Vertices[2], InColor);
	AddLine(Vertices[2], Vertices[3], InColor);
	AddLine(Vertices[3], Vertices[0], InColor);

	// 위쪽 면 
	AddLine(Vertices[4], Vertices[5], InColor);
	AddLine(Vertices[5], Vertices[6], InColor);
	AddLine(Vertices[6], Vertices[7], InColor);
	AddLine(Vertices[7], Vertices[4], InColor);

	// 옆면
	AddLine(Vertices[0], Vertices[4], InColor);
	AddLine(Vertices[1], Vertices[5], InColor);
	AddLine(Vertices[2], Vertices[6], InColor);
	AddLine(Vertices[3], Vertices[7], InColor);
}

const TArray<FDebugLine>& FDebugDrawManager::GetLines() const
{
	return DebugLines;
}

void FDebugDrawManager::ClearLines()
{
	DebugLines.clear();
}
