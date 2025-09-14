#include "pch.h"
#include "Name.h"

TArray<FNameEntry> FNamePool::Entries;

uint32 FNamePool::Add(const FString& InStr)
{
    FString Lower = InStr;
    std::transform(Lower.begin(), Lower.end(), Lower.begin(), ::tolower);

    for (uint32 i = 0; i < (uint32)Entries.size(); ++i)
        if (Entries[i].Comparison == Lower)
            return i;

    Entries.push_back({ InStr, Lower });
    return (uint32)Entries.size() - 1;
}

const FNameEntry& FNamePool::Get(uint32 Index)
{
    return Entries[Index];
}