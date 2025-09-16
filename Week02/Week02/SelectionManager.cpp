#include "pch.h"
#include "SelectionManager.h"
#include "Actor.h"

USelectionManager& USelectionManager::GetInstance()
{
    static USelectionManager* Instance = nullptr;
    if (Instance == nullptr)
    {
        Instance = NewObject<USelectionManager>();
    }
    return *Instance;
}

void USelectionManager::SelectActor(AActor* Actor)
{
    if (!Actor) return;
    
    // 이미 선택되어 있는지 확인
    if (IsActorSelected(Actor)) return;
    
    // 단일 선택 모드 (기존 선택 해제)
    ClearSelection();
    
    // 새 액터 선택
    SelectedActors.Add(Actor);
}

void USelectionManager::DeselectActor(AActor* Actor)
{
    if (!Actor) return;
    
    auto it = std::find(SelectedActors.begin(), SelectedActors.end(), Actor);
    if (it != SelectedActors.end())
    {
        SelectedActors.erase(it);
    }
}

void USelectionManager::ClearSelection()
{
    for (AActor* Actor : SelectedActors)
    {
        Actor->SetIsPicked(false);
    }
    SelectedActors.clear();
}

bool USelectionManager::IsActorSelected(AActor* Actor) const
{
    if (!Actor) return false;
    
    return std::find(SelectedActors.begin(), SelectedActors.end(), Actor) != SelectedActors.end();
}

AActor* USelectionManager::GetSelectedActor() const
{
    return HasSelection() ? SelectedActors[0] : nullptr;
}

USelectionManager::USelectionManager()
{
    SelectedActors.Reserve(1);
}

USelectionManager::~USelectionManager()
{
    // No-op: instances are destroyed by ObjectFactory::DeleteAll
}
