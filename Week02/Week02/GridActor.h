#pragma once
#include "Actor.h"
#include "LineComponent.h"

class AGridActor : public AActor
{
public:
    DECLARE_CLASS(AGridActor, AActor);
    AGridActor();

protected:
    ~AGridActor() override;

public:
    // Grid and Axis creation methods
    void CreateGridLines(int32 GridSize = 50, float CellSize = 1.0f, const FVector& Center = FVector());
    void CreateAxisLines(float Length = 50.0f, const FVector& Origin = FVector());
    void ClearLines();
    
    // Grid settings
    void SetGridSize(int32 NewGridSize) { GridSize = NewGridSize; RegenerateGrid(); }
    void SetCellSize(float NewCellSize) { CellSize = NewCellSize; RegenerateGrid(); }
    void SetAxisLength(float NewLength) { AxisLength = NewLength; RegenerateGrid(); }
    
    int32 GetGridSize() const { return GridSize; }
    float GetCellSize() const { return CellSize; }
    float GetAxisLength() const { return AxisLength; }
    
    // Component access
    ULineComponent* GetLineComponent() const { return LineComponent; }

private:
    void RegenerateGrid();
    
    ULineComponent* LineComponent;
    
    // Grid settings
    int32 GridSize = 50;
    float CellSize = 1.0f;
    float AxisLength = 50.0f;
};

