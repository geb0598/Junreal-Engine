﻿#include "pch.h"
#include "LineComponent.h"
#include "Renderer.h"

void ULineComponent::GetWorldLineData(TArray<FVector>& OutStartPoints, TArray<FVector>& OutEndPoints, TArray<FVector4>& OutColors) const
{
    if (!bLinesVisible || Lines.empty())
    {
        OutStartPoints.clear();
        OutEndPoints.clear();
        OutColors.clear();
        return;
    }
    
    FMatrix worldMatrix = GetWorldMatrix();
    size_t lineCount = Lines.size();
    
    for (const ULine* Line : Lines)
    {
        if (Line)
        {
            FVector worldStart, worldEnd;
            Line->GetWorldPoints(worldMatrix, worldStart, worldEnd);
            
            OutStartPoints.push_back(worldStart);
            OutEndPoints.push_back(worldEnd);
            OutColors.push_back(Line->GetColor());
        }
    }
    return;
}

ULineComponent::ULineComponent()
{
    bLinesVisible = true;
}

ULineComponent::~ULineComponent()
{
    ClearLines();
}

ULine* ULineComponent::AddLine(const FVector& StartPoint, const FVector& EndPoint, const FVector4& Color)
{
    ULine* NewLine = NewObject<ULine>();
    NewLine->SetLine(StartPoint, EndPoint);
    NewLine->SetColor(Color);
    
    Lines.push_back(NewLine);
    
    return NewLine;
}

void ULineComponent::RemoveLine(ULine* Line)
{
    if (!Line) return;
    
    auto it = std::find(Lines.begin(), Lines.end(), Line);
    if (it != Lines.end())
    {
        DeleteObject(*it);
        Lines.erase(it);
    }
}

void ULineComponent::ClearLines()
{
    for (ULine* Line : Lines)
    {
        if (Line)
        {
            DeleteObject(Line);
        }
    }
    Lines.Empty();
}
const FAABB ULineComponent::GetWorldAABB() const
{
    TArray<FVector> OutStart;
    TArray<FVector> OutEnd;
    TArray<FVector4> OutColor;
    GetWorldLineData(OutStart, OutEnd, OutColor);
    TArray<FVector> Vertices;
    Vertices.Append(OutStart);
    Vertices.Append(OutEnd);
    return FAABB(Vertices);
}

void ULineComponent::Render(URenderer* Renderer, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, const EEngineShowFlags ShowFlag)
{
    if (!HasVisibleLines() || !Renderer)
        return;

    TArray<FVector> startPoints, endPoints;
    TArray<FVector4> colors;
    
    // Extract world coordinate line data efficiently
    GetWorldLineData(startPoints, endPoints, colors);
    
    // Add all lines to renderer batch at once
    if (!startPoints.empty())
    {
        Renderer->AddLines(startPoints, endPoints, colors);
    }
    startPoints.clear();
    startPoints.Shrink();
    endPoints.clear();
    endPoints.Shrink();
    colors.clear();
    colors.Shrink();
}

