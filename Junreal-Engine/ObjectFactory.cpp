﻿#include "pch.h"
#include "ObjectFactory.h"

// 전역 오브젝트 배열 정의 (한 번만!)
TArray<UObject*> GUObjectArray;
TArray<int32>    GFreeIndices; // 빈 슬롯 목록
namespace ObjectFactory
{
    TMap<UClass*, ConstructFunc>& GetRegistry()
    {
        static TMap<UClass*, ConstructFunc> Registry;
        return Registry;
    }

    TMap<FString, UClass*>& GetNameRegistry()
    {
        static TMap<FString, UClass*> NameRegistry;
        return NameRegistry;
    }

    void RegisterClassType(UClass* Class, ConstructFunc Func)
    {
        GetRegistry()[Class] = std::move(Func);
        // 클래스 이름으로도 등록
        if (Class && Class->Name)
        {
            GetNameRegistry()[Class->Name] = Class;
        }
    }

    UClass* FindClassByName(const FString& ClassName)
    {
        auto& nameReg = GetNameRegistry();
        auto it = nameReg.find(ClassName);
        if (it == nameReg.end()) return nullptr;
        return it->second;
    }

    UObject* NewObject(const FString& ClassName)
    {
        UClass* Class = FindClassByName(ClassName);
        if (!Class)
        {
            UE_LOG("ObjectFactory: Class not found: %s", ClassName.c_str());
            return nullptr;
        }
        return NewObject(Class);
    }

    UObject* ConstructObject(UClass* Class)
    {
        auto& reg = GetRegistry();
        auto it = reg.find(Class);
        if (it == reg.end()) return nullptr;
        return it->second();
    }
    
    UObject* NewObject(UClass* Class)
    {
        UObject* Obj = ConstructObject(Class);
        if (!Obj) return nullptr;

        int32 idx = -1;
        if (GFreeIndices.Num() > 0)
        {
            // 빈 슬롯 재사용
            idx = GFreeIndices.Last();
            GFreeIndices.Pop();
            GUObjectArray[idx] = Obj;
        }
        else
        {
            // 빈 슬롯 없으면 새로 push
            idx = GUObjectArray.Add(Obj);
        }

        Obj->InternalIndex = static_cast<uint32>(idx);

        // 고유 이름 부여
        static TMap<UClass*, int> NameCounters;
        int Count = ++NameCounters[Class];

        const std::string base = Class->Name;
        std::string unique;
        unique.reserve(base.size() + 12);
        unique.append(base);
        unique.push_back('_');
        unique.append(std::to_string(Count));

        Obj->ObjectName = FName(unique);

        return Obj;
    }

    UObject* NewObject(UObject* Outer, UClass* Class)
    {
        UObject* Obj = ConstructObject(Class);
        if (!Obj) return nullptr;

        // Outer 설정
        Obj->Outer = Outer;

        int32 idx = -1;
        if (GFreeIndices.Num() > 0)
        {
            // 빈 슬롯 재사용
            idx = GFreeIndices.Last();
            GFreeIndices.Pop();
            GUObjectArray[idx] = Obj;
        }
        else
        {
            // 빈 슬롯 없으면 새로 push
            idx = GUObjectArray.Add(Obj);
        }

        Obj->InternalIndex = static_cast<uint32>(idx);

        // 고유 이름 부여
        static TMap<UClass*, int> NameCounters;
        int Count = ++NameCounters[Class];

        const std::string base = Class->Name;
        std::string unique;
        unique.reserve(base.size() + 12);
        unique.append(base);
        unique.push_back('_');
        unique.append(std::to_string(Count));

        Obj->ObjectName = FName(unique);

        return Obj;
    }

    void DeleteObject(UObject* Obj)
    {
        if (!Obj) return;

        // Important: DO NOT dereference Obj fields before verifying it is still in GUObjectArray.
        int32 foundIndex = -1;
        for (int32 i = 0; i < GUObjectArray.Num(); ++i)
        {
            if (GUObjectArray[i] == Obj)
            {
                foundIndex = i;
                break;
            }
        }
        if (foundIndex < 0)
        {
            // Not managed or already deleted.
            return;
        }

        GUObjectArray[foundIndex] = nullptr;
        // Safe to delete now; Obj still valid since we found it in GUObjectArray
        Obj->DestroyInternal();
    }

    void DeleteAll(bool bCallBeginDestroy)
    {
        // 실제 삭제 (역순 안전)
        for (int32 i = GUObjectArray.Num() - 1; i >= 0; --i)
        {
            if (UObject* Obj = GUObjectArray[i])
            {
                DeleteObject(Obj);
            }
        }
        GUObjectArray.Empty();
        GUObjectArray.Shrink();
        //GUObjectArray.clear();
    }
    // (선택) null 슬롯 압축
    void CompactNullSlots()
    {
        int32 write = 0;
        for (int32 read = 0; read < GUObjectArray.Num(); ++read)
        {
            if (UObject* Obj = GUObjectArray[read])
            {
                if (write != read)
                {
                    GUObjectArray[write] = Obj;
                    Obj->InternalIndex = static_cast<uint32>(write);
                    GUObjectArray[read] = nullptr;
                }
                ++write;
            }
        }
        // 크기(Num) 축소 + 불필요한 capacity도 반환
        GUObjectArray.SetNum(write);
    }
}
