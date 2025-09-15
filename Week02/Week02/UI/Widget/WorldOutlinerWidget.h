#pragma once
#include "Widget.h"
#include "../../Vector.h"
#include "../../UEContainer.h"

class UUIManager;
class UWorld;
class AActor;
class USelectionManager;

/**
 * WorldOutlinerWidget
 * - Unreal Engine style hierarchical object browser
 * - Shows all actors in the world in a tree view
 * - Supports selection, visibility toggle, hierarchy management
 * - Syncs with 3D viewport selection
 */
class UWorldOutlinerWidget : public UWidget
{
public:
    DECLARE_CLASS(UWorldOutlinerWidget, UWidget)
    
    void Initialize() override;
    void Update() override;
    void RenderWidget() override;
    
    // Special Member Functions
    UWorldOutlinerWidget();
    ~UWorldOutlinerWidget() override;

private:
    UUIManager* UIManager = nullptr;
    USelectionManager* SelectionManager = nullptr;
    
    // UI State
    bool bShowOnlySelectedObjects = false;
    bool bShowHiddenObjects = true;
    FString SearchFilter = "";
    
    // Actor Management
    struct FActorTreeNode
    {
        AActor* Actor = nullptr;
        TArray<FActorTreeNode*> Children;
        FActorTreeNode* Parent = nullptr;
        bool bIsExpanded = true;
        bool bIsVisible = true;
        
        FActorTreeNode(AActor* InActor) : Actor(InActor) {}
        ~FActorTreeNode() 
        {
            for (auto* Child : Children)
            {
                delete Child;
            }
        }
    };
    
    TArray<FActorTreeNode*> RootNodes;
    
    // Helper Methods
    UWorld* GetCurrentWorld() const;
    void RefreshActorTree();
    void BuildActorHierarchy();
    void RenderActorNode(FActorTreeNode* Node, int32 Depth = 0);
    bool ShouldShowActor(AActor* Actor) const;
    void HandleActorSelection(AActor* Actor);
    void HandleActorVisibilityToggle(AActor* Actor);
    void HandleActorRename(AActor* Actor);
    void HandleActorDelete(AActor* Actor);
    void HandleActorDuplicate(AActor* Actor);
    
    // Context Menu
    AActor* ContextMenuTarget = nullptr;
    bool bShowContextMenu = false;
    void RenderContextMenu();
    
    // Drag & Drop (for hierarchy management)
    AActor* DragSource = nullptr;
    AActor* DropTarget = nullptr;
    void HandleDragDrop();
    
    // Search & Filter
    void RenderSearchBar();
    bool PassesSearchFilter(AActor* Actor) const;
    
    // Toolbar
    void RenderToolbar();
    
    // Tree management
    void ClearActorTree();
    FActorTreeNode* FindNodeByActor(AActor* Actor);
    void ExpandParentsOfSelected();
    
    // Selection synchronization
    void SyncSelectionFromViewport();
    void SyncSelectionToViewport(AActor* Actor);
    
    // Object creation
    void CreateNewActor(const FString& ActorType);
};