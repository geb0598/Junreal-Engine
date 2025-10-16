﻿#pragma once
#include "UEContainer.h"
#include "ObjectFactory.h"
#include "MemoryManager.h"
#include "Name.h"
#include "Enums.h"
#include "PropertyFlag.h"

// 전방 선언/외부 심볼 (네 프로젝트 환경 유지)
class UObject;
class UWorld;
// ── UClass: 간단한 타입 디스크립터 ─────────────────────────────
struct UClass
{
    const char* Name = nullptr;
    const UClass* Super = nullptr;   // 루트(UObject)는 nullptr
    std::size_t   Size = 0;
    UObject* (*CreateInstance)() = nullptr; // 인스턴스 생성 함수 포인터

    constexpr UClass() = default;
    constexpr UClass(const char* n, const UClass* s, std::size_t z, UObject* (*creator)() = nullptr)
        :Name(n), Super(s), Size(z), CreateInstance(creator) {
    }
    bool IsChildOf(const UClass* Base) const noexcept
    {
        if (!Base) return false;
        for (auto c = this; c; c = c->Super)
            if (c == Base) return true;
        return false;
    }
};

// ── 글로벌 클래스 레지스트리 ─────────────────────────────
class UClassRegistry
{
public:
    static UClassRegistry& Get()
    {
        static UClassRegistry Instance;
        return Instance;
    }

    void RegisterClass(UClass* InClass)
    {
        if (InClass)
        {
            RegisteredClasses.push_back(InClass);
        }
    }

    const TArray<UClass*>& GetAllClasses() const
    {
        return RegisteredClasses;
    }

    // 특정 베이스 클래스의 모든 자식 클래스 가져오기
    TArray<UClass*> GetDerivedClasses(const UClass* BaseClass) const
    {
        TArray<UClass*> Result;
        for (UClass* Class : RegisteredClasses)
        {
            if (Class && Class->IsChildOf(BaseClass) && Class != BaseClass)
            {
                Result.push_back(Class);
            }
        }
        return Result;
    }

private:
    TArray<UClass*> RegisteredClasses;
};

class UObject
{
public:
    UObject() : UUID(GenerateUUID()), InternalIndex(UINT32_MAX), ObjectName("UObject") {}

protected:
    virtual ~UObject() = default;
    // Centralized deletion entry accessible to ObjectFactory only
    void DestroyInternal() { delete this; }
    friend void ObjectFactory::DeleteObject(UObject* Obj);

public:
    // UObject-scoped allocation only
    static void* operator new(std::size_t size) { return CMemoryManager::Allocate(size); }
    static void  operator delete(void* ptr) noexcept { CMemoryManager::Deallocate(ptr); }
    static void  operator delete(void* ptr, std::size_t) noexcept { CMemoryManager::Deallocate(ptr); }

    FString GetName();    // 원문
    FString GetComparisonName(); // lower-case

    UObject* GetOuter() const;
    virtual UWorld* GetWorld() const;

    // Duplicate for PIE
    template<typename T>
    T* Duplicate();
    
    virtual UObject* Duplicate();
    virtual void DuplicateSubObjects();

public:

    // [PIE] ???
    uint32_t UUID;
    uint32_t InternalIndex;
    // [PIE] 값 복사
    FName    ObjectName;   // ← 객체 개별 이름 추가
    UObject* Outer = nullptr;  // ← Outer 객체 참조

public:
    // 정적: 타입 메타 반환 (이름을 StaticClass로!)
    static UClass* StaticClass()
    {
        static UClass Cls{ "UObject", nullptr, sizeof(UObject), nullptr };
        static bool bRegistered = false;
        if (!bRegistered)
        {
            UClassRegistry::Get().RegisterClass(&Cls);
            bRegistered = true;
        }
        return &Cls;
    }

    // 가상: 인스턴스의 실제 타입 메타
    virtual UClass* GetClass() const { return StaticClass(); }

    // IsA 헬퍼
    bool IsA(const UClass* C) const noexcept { return GetClass()->IsChildOf(C); }
    template<class T> bool IsA() const noexcept { return IsA(T::StaticClass()); }

    // 다음으로 발급될 UUID를 조회 (증가 없음)
    static uint32 PeekNextUUID() { return GUUIDCounter; }

    // 다음으로 발급될 UUID를 설정 (예: 씬 로드시 메타와 동기화)
    static void SetNextUUID(uint32 Next) { GUUIDCounter = Next; }

    // UUID 발급기: 현재 카운터를 반환하고 1 증가
    static uint32 GenerateUUID() { return GUUIDCounter++; }
    
    //static EPropertyFlag GetPropertyFlag() { return EPropertyFlag::CPF_Instanced; }

private:
    // 전역 UUID 카운터(초기값 1)
    inline static uint32 GUUIDCounter = 1;
};

// ── Cast 헬퍼 (UE Cast<> 와 동일 UX) ────────────────────────────
template<class T>
T* Cast(UObject* Obj) noexcept
{
    return (Obj && Obj->IsA<T>()) ? static_cast<T*>(Obj) : nullptr;
}
template<class T>
const T* Cast(const UObject* Obj) noexcept
{
    return (Obj && Obj->IsA<T>()) ? static_cast<const T*>(Obj) : nullptr;
}

// ── 파생 타입에 붙일 매크로 ─────────────────────────────────────
#define DECLARE_CLASS(ThisClass, SuperClass)                                  \
public:                                                                       \
    using Super_t = SuperClass;                                               \
    static UObject* CreateInstance() { return new ThisClass(); }              \
    static UClass* StaticClass()                                              \
    {                                                                         \
        static UClass Cls{ #ThisClass, SuperClass::StaticClass(),             \
                            sizeof(ThisClass), &ThisClass::CreateInstance };  \
        static bool bRegistered = false;                                      \
        if (!bRegistered)                                                     \
        {                                                                     \
            UClassRegistry::Get().RegisterClass(&Cls);                        \
            bRegistered = true;                                               \
        }                                                                     \
        return &Cls;                                                          \
    }                                                                         \
    virtual UClass* GetClass() const override { return ThisClass::StaticClass(); }

// ── 추상 클래스용 매크로 (인스턴스 생성 불가) ─────────────────────
#define DECLARE_ABSTRACT_CLASS(ThisClass, SuperClass)                         \
public:                                                                       \
    using Super_t = SuperClass;                                               \
    static UClass* StaticClass()                                              \
    {                                                                         \
        static UClass Cls{ #ThisClass, SuperClass::StaticClass(),             \
                            sizeof(ThisClass), nullptr };                     \
        static bool bRegistered = false;                                      \
        if (!bRegistered)                                                     \
        {                                                                     \
            UClassRegistry::Get().RegisterClass(&Cls);                        \
            bRegistered = true;                                               \
        }                                                                     \
        return &Cls;                                                          \
    }                                                                         \
    virtual UClass* GetClass() const override { return ThisClass::StaticClass(); }

//template<typename T>
//T* UObject::Duplicate()
//{
//    T* NewObject;
//    
//    EPropertyFlag EffectiveFlags = T::GetPropertyFlag();
//    
//    if ((EffectiveFlags & EPropertyFlag::CPF_DuplicateTransient) != EPropertyFlag::CPF_None)
//    {
//        // New Instance: 기본 생성자로 새 객체 생성
//        NewObject = new T();
//    }
//    else if ((EffectiveFlags & EPropertyFlag::CPF_EditAnywhere) != EPropertyFlag::CPF_None)
//    {
//        // Shallow Copy: 에디터에서 편집 가능한 얕은 복사
//        NewObject = new T(*static_cast<T*>(this));
//    }
//    else if ((EffectiveFlags & EPropertyFlag::CPF_Instanced) != EPropertyFlag::CPF_None)
//    {
//        // Deep Copy: 참조된 객체도 Deep Copy
//        NewObject = new T(*static_cast<T*>(this));
//        NewObject->DuplicateSubObjects();
//    }
//    else // 기본값: Deep Copy
//    {
//        NewObject = new T(*static_cast<T*>(this));
//        NewObject->DuplicateSubObjects();
//    }
//    
//    return NewObject;
//}
