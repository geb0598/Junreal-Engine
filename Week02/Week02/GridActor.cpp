#include "pch.h"
#include "GridActor.h"

AGridActor::AGridActor()
{
    // Create LineComponent for rendering grid and axis lines
    LineComponent = NewObject<ULineComponent>();
    LineComponent->SetupAttachment(RootComponent);
    AddComponent(LineComponent);
    
    // Generate initial grid and axis lines
    RegenerateGrid();
}

AGridActor::~AGridActor()
{

}

void AGridActor::CreateGridLines(int32 InGridSize, float InCellSize, const FVector& Center)
{
    if (!LineComponent) return;
    
    // Z축 방향 선들
    for (int i = -InGridSize; i <= InGridSize; i++)
    {
        if (i == 0) continue; // 중앙축은 따로 처리
        
        float x = i * InCellSize;
        FVector4 color;
        
        // 색상 결정: 10의 배수면 흰색, 5의 배수면 밝은 회색, 아니면 어두운 회색
        if (i % 10 == 0)
            color = FVector4(1.0f, 1.0f, 1.0f, 1.0f);      // 흰색
        else if (i % 5 == 0)
            color = FVector4(0.4f, 0.4f, 0.4f, 1.0f);      // 밝은 회색
        else
            color = FVector4(0.1f, 0.1f, 0.1f, 1.0f);      // 어두운 회색
        
        // Z축 방향 라인: (i, 0, -N) ~ (i, 0, +N)
        FVector start(x, 0.0f, -InGridSize * InCellSize);
        FVector end(x, 0.0f, InGridSize * InCellSize);
        LineComponent->AddLine(start, end, color);
    }
    
    // X축 방향 선들
    for (int j = -InGridSize; j <= InGridSize; j++)
    {
        if (j == 0) continue; // 중앙축은 따로 처리
        
        float z = j * InCellSize;
        FVector4 color;
        
        // 색상 결정
        if (j % 10 == 0)
            color = FVector4(1.0f, 1.0f, 1.0f, 1.0f);      // 흰색
        else if (j % 5 == 0)
            color = FVector4(0.4f, 0.4f, 0.4f, 1.0f);      // 밝은 회색
        else
            color = FVector4(0.1f, 0.1f, 0.1f, 1.0f);      // 어두운 회색
        
        // X축 방향 라인: (-N, 0, j) ~ (+N, 0, j)
        FVector start(-InGridSize * InCellSize, 0.0f, z);
        FVector end(InGridSize * InCellSize, 0.0f, z);
        LineComponent->AddLine(start, end, color);
    }
    
    // Z축 중앙 라인 (X=0)
    FVector start_z(0.0f, 0.0f, -InGridSize * InCellSize);
    FVector end_z(0.0f, 0.0f, 0.0f);
    LineComponent->AddLine(start_z, end_z, FVector4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // X축 중앙 라인 (Z=0)
    FVector start_x(-InGridSize * InCellSize, 0.0f, 0.0f);
    FVector end_x(0.0f, 0.0f, 0.0f);
    LineComponent->AddLine(start_x, end_x, FVector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void AGridActor::CreateAxisLines(float Length, const FVector& Origin)
{
    if (!LineComponent) return;
        
    // X축 (빨강) - 원점에서 +X 방향
    LineComponent->AddLine(Origin, 
                          Origin + FVector(Length, 0.0f, 0.0f), 
                          FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    
    // Y축 (초록) - 원점에서 +Y 방향
    LineComponent->AddLine(Origin, 
                          Origin + FVector(0.0f, Length, 0.0f), 
                          FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    
    // Z축 (파랑) - 원점에서 +Z 방향
    LineComponent->AddLine(Origin, 
                          Origin + FVector(0.0f, 0.0f, Length), 
                          FVector4(0.0f, 0.0f, 1.0f, 1.0f));
}

void AGridActor::ClearLines()
{
    if (LineComponent)
    {
        LineComponent->ClearLines();
    }
}

void AGridActor::RegenerateGrid()
{
    // Clear existing lines
    ClearLines();
    
    // Generate new grid and axis lines with current settings
    CreateGridLines(GridSize, CellSize, FVector());
    CreateAxisLines(AxisLength, FVector());
}

