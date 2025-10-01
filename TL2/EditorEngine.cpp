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
    for (auto& Context : WorldContexts)
    {
        UWorld* World = Context.World();
        if (!World) continue;

        if (Context.WorldType == EWorldType::Editor)
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
    for (auto& Context : WorldContexts)
    {
        UWorld* World = Context.World();
        if (!World) continue;

        if (Context.WorldType == EWorldType::Editor)
        {
            World->Render();
        }
    }

    // PIE가 돌고 있으면 같이 Tick
    if (GameEngine)
        GameEngine->Render();
}

void UEditorEngine::StartPIE()
{
    /*UWorld* EditorWorld = GetWorld(EWorldType::Editor);
    if (!EditorWorld) return;


    UWorld* PIEWorld = UWorld::DuplicateWorldForPIE(EditorWorld, ...);
    if (!PIEWorld) return;

    FWorldContext PieCtx;
    PieCtx.SetWorld(PIEWorld, EWorldType::PIE);
    WorldContexts.push_back(PieCtx);

    GameEngine = new UGameEngine();
    GameEngine->StartGame(PIEWorld);*/
}

void UEditorEngine::EndPIE()
{
    if (GameEngine)
    {
        GameEngine->EndGame();
        delete GameEngine;
        GameEngine = nullptr;
    }

    // PIEWorld 컨텍스트 제거
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

