#pragma once
#include"pch.h"
#include "EditorEngine.h"
#include"GameEngine.h"
#include "World.h"

UEditorEngine::UEditorEngine()
{
}

void UEditorEngine::Tick(float DeltaSeconds)
{
    // 인덱스 기반 루프로 iterator 무효화 방지
    for (size_t i = 0; i < WorldContexts.size(); ++i)
    {
        UWorld* World = WorldContexts[i].World();
        if (!World) continue;

        if (WorldContexts[i].WorldType == EWorldType::Editor)
        {
            World->Tick(DeltaSeconds);
        }
    }

    // PIE가 돌고 있으면 같이 Tick
    if (GameEngine)
        GameEngine->Tick(DeltaSeconds);
}

void UEditorEngine::Render()
{
    // PIE 실행 중이면 PIE 월드만 렌더링
    if (GameEngine && GameEngine->GameWorld)
    {
        GameEngine->Render();
    }
    else
    {
        // 에디터 모드일 때만 에디터 월드 렌더링
        for (size_t i = 0; i < WorldContexts.size(); ++i)
        {
            UWorld* World = WorldContexts[i].World();
            if (!World) continue;

            if (WorldContexts[i].WorldType == EWorldType::Editor)
            {
                World->Render();
            }
        }
    }
}

void UEditorEngine::StartPIE()
{
    UWorld* EditorWorld = GetWorld(EWorldType::Editor);
    if (!EditorWorld) return;

    UWorld* PIEWorld = UWorld::DuplicateWorldForPIE(EditorWorld);
    if (!PIEWorld) return;

    FWorldContext PieCtx;
    PieCtx.SetWorld(PIEWorld, EWorldType::PIE);
    WorldContexts.push_back(PieCtx);

    GameEngine = NewObject<UGameEngine>();
    GameEngine->StartGame(PIEWorld);
}

void UEditorEngine::EndPIE()
{
    UWorld* PIEWorld = nullptr;

    // PIE 월드 찾기
    for (auto& Context : WorldContexts)
    {
        if (Context.WorldType == EWorldType::PIE)
        {
            PIEWorld = Context.World();
            break;
        }
    }

    // GameEngine 정리
    if (GameEngine)
    {
        GameEngine->EndGame();  // CleanupWorld 호출
        delete GameEngine;
        GameEngine = nullptr;
    }

    // PIE 월드 삭제
    if (PIEWorld)
    {
        ObjectFactory::DeleteObject(PIEWorld);
    }

    // PIE WorldContext 제거
    for (int i = (int)WorldContexts.size() - 1; i >= 0; --i)
    {
        if (WorldContexts[i].WorldType == EWorldType::PIE)
        {
            WorldContexts.erase(WorldContexts.begin() + i);
            break;
        }
    }
}

UEditorEngine::~UEditorEngine()
{
}

