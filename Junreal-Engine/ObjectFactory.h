﻿#pragma once
#include "UEContainer.h"


// ── 외부 심볼 ─────────────────────────────────────────────
class UObject;
struct UClass;
extern TArray<UObject*> GUObjectArray;

// ── ObjectFactory 네임스페이스 ─────────────────────────────
namespace ObjectFactory
{
    using ConstructFunc = std::function<UObject* ()>;

    // Registry 접근자
    TMap<UClass*, ConstructFunc>& GetRegistry();
    TMap<FString, UClass*>& GetNameRegistry();

    // 클래스 등록
    void RegisterClassType(UClass* Class, ConstructFunc Func);

    // 클래스 이름으로 UClass 찾기
    UClass* FindClassByName(const FString& ClassName);

    // 1) 순수 생성 (GUObjectArray 등록 X)
    UObject* ConstructObject(UClass* Class);

    // 2) 생성 + GUObjectArray 자동 등록
    UObject* NewObject(UClass* Class);
    UObject* NewObject(UObject* Outer, UClass* Class);
    UObject* NewObject(const FString& ClassName);

    // 3) 템플릿 버전 (타입 안전)
    template<class T>
    inline T* NewObject()
    {
        return static_cast<T*>(NewObject(T::StaticClass()));
    }

    template<class T>
    inline T* NewObject(UObject& Outer)
    {
        return static_cast<T*>(NewObject(&Outer, T::StaticClass()));
    }
    // 개별 삭제(단일 소유자: Factory)
    void DeleteObject(UObject* Obj);
    // 종료시 일괄 정리
    void DeleteAll(bool bCallBeginDestroy = true);
    // Null 슬롯 압축하여 배열 크기 축소
    void CompactNullSlots();
}

// ── 등록 매크로 ─────────────────────────────────────────────
#define IMPLEMENT_CLASS(ThisClass)                                            \
    namespace {                                                               \
        struct ThisClass##FactoryRegister                                     \
        {                                                                     \
            ThisClass##FactoryRegister()                                      \
            {                                                                 \
                ObjectFactory::RegisterClassType(                             \
                    ThisClass::StaticClass(),                                 \
                    []() -> UObject* { return new ThisClass(); }              \
                );                                                            \
            }                                                                 \
        };                                                                    \
        static ThisClass##FactoryRegister GRegister_##ThisClass;              \
    }
using namespace ObjectFactory;
