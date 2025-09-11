#pragma once
#include "Object.h"
#include "Enums.h"

class UStaticMesh : public UObject
{
public:
    DECLARE_CLASS(UStaticMesh, UObject)
    UStaticMesh();

protected:
    ~UStaticMesh() override;

public:
    FString& GetFilePath() { return FilePath; }
    void SetFilePath(const FString& InFilePath);
    void SetResourceData(FResourceData* InData) { ResourceData = InData; }
    FResourceData* GetResourceData() { return ResourceData; }
protected:
    FString FilePath;
    FResourceData* ResourceData = nullptr;
};

