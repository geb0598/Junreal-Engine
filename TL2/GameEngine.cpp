#pragma once
#include"pch.h"
#include "GameEngine.h"
#include "World.h"

UGameEngine::UGameEngine()
{
}

void UGameEngine::Tick(float DeltaSeconds)
{
    if (GameWorld)
        GameWorld->Tick(DeltaSeconds);
}

void UGameEngine::Render()
{
    if (GameWorld)
        GameWorld->Render();
}

void UGameEngine::StartGame(UWorld* World)
{
    GameWorld = World;
    if (GameWorld)
    {
        // 모든 액터에 BeginPlay 같은 초기화 호출
      //  GameWorld->InitializeActorsForPlay();
    }
}

void UGameEngine::EndGame()
{
  /*  if (GameWorld)
    {
        GameWorld->CleanupWorld();
        delete GameWorld;
        GameWorld = nullptr;
    }*/
}

UGameEngine::~UGameEngine()
{
}
