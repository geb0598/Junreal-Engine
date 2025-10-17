#include "pch.h"
#include "Resource/DebugDrawManager.h"


FDebugDrawManager& FDebugDrawManager::GetInstance()
{
	static FDebugDrawManager Instance;
	return Instance;
}

void FDebugDrawManager::AddLine(const FVector& Start, const FVector& End, const FVector4& Color)
{
	DebugLines.Add(FDebugLine{ Start, End, Color });
}

void FDebugDrawManager::AddCircle(const FVector& Center, const FVector& UpAxis, float Radius, int NumSegments, const FVector4& Color)
{
	if (NumSegments <= 2)
	{
		return;
	}
	FVector AxisU, AxisV;
	UpAxis.FindBestAxisVectors(AxisU, AxisV);

	FVector LastVertex = Center + AxisU * Radius;
	for (int i = 1;i <= NumSegments; ++i)
	{
		float Angle = i / NumSegments * 2.0f * PI;
		FVector Vertex = Center + (AxisU * cos(Angle) + AxisV * sin(Angle)) * Radius;
		DebugLines.Add(FDebugLine{ LastVertex, Vertex, Color });
		LastVertex = Vertex;
	}
}

void FDebugDrawManager::AddSphere(const FVector& Center, float Radius, int NumSegments, const FVector4& Color)
{
	AddCircle(Center, FVector(1.0f, 0.0f, 0.0f), Radius, NumSegments, Color); // XY 평면
	AddCircle(Center, FVector(0.0f, 1.0f, 0.0f), Radius, NumSegments, Color); // XZ 평면
	AddCircle(Center, FVector(0.0f, 0.0f, 1.0f), Radius, NumSegments, Color); // YZ 평면
}

void FDebugDrawManager::AddCone(const FVector& ApexPosition, const FVector& Direction, float Height, float AngleInDegrees, int NumSegments, const FVector4& Color)
{
	if (NumSegments < 3 || Height <= 0.0f) return;

	// 원뿔의 기본 정보
	const FVector DirNorm = Direction.GetSafeNormal();
	const FVector BaseCenter = ApexPosition + DirNorm * Height;
	const float AngleInRadians = DegreeToRadian(AngleInDegrees);
	const float BaseRadius = tanf(AngleInRadians) * Height;

	// 밑면 원 그리기
	AddCircle(BaseCenter, DirNorm, BaseRadius, NumSegments, Color);

	// 꼭짓점에서 밑면으로 이어지는 선들 그리기
	FVector AxisU, AxisV;
	DirNorm.FindBestAxisVectors(AxisU, AxisV);

	// 일단, 4개의 옆면 선만 그림
	for (int i = 0; i < 4; ++i)
	{
		float Angle = static_cast<float>(i) / 4.0f * TWO_PI; // 0, 90, 180, 270도
		FVector PointOnCircle = BaseCenter + (AxisU * cosf(Angle) + AxisV * sinf(Angle)) * BaseRadius;
		AddLine(ApexPosition, PointOnCircle, Color);
	}
}



void FDebugDrawManager::AddBox(const FVector& Extents, const FVector4& Color)
{
}

const TArray<FDebugLine>& FDebugDrawManager::GetLines() const
{
	return DebugLines;
}

void FDebugDrawManager::ClearLines()
{
	DebugLines.clear();
}
