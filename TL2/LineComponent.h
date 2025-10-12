﻿#pragma once
#include "PrimitiveComponent.h"
#include "Line.h"
#include "UEContainer.h"

class URenderer;

class ULineComponent : public UPrimitiveComponent
{
public:
    DECLARE_CLASS(ULineComponent, UPrimitiveComponent)
    
    ULineComponent();
    virtual ~ULineComponent() override;

public:
    // Line management
    ULine* AddLine(const FVector& StartPoint, const FVector& EndPoint, const FVector4& Color = FVector4(1,1,1,1));
    void RemoveLine(ULine* Line);
    void ClearLines();

    virtual void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;

    // Properties
    void SetLineVisible(bool bVisible) { bLinesVisible = bVisible; }
    bool IsLineVisible() const { return bLinesVisible; }
    
    const TArray<ULine*>& GetLines() const { return Lines; }
    int64 GetLineCount() const { return static_cast<int64>(Lines.size()); }
    
    // Efficient world coordinate line data extraction
    void GetWorldLineData(TArray<FVector>& OutStartPoints, TArray<FVector>& OutEndPoints, TArray<FVector4>& OutColors) const;
    bool HasVisibleLines() const { return bLinesVisible && !Lines.empty(); }
    const FAABB GetWorldAABB() const override;

private:
    TArray<ULine*> Lines;
    bool bLinesVisible = true;
 };
