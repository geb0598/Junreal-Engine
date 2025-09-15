#include "pch.h"
#include "WorldOutlinerWidget.h"
#include "../UIManager.h"
#include "../../ImGui/imgui.h"
#include "../../World.h"
#include "../../Actor.h"
#include "../../SelectionManager.h"
#include <algorithm>
#include <string>

// UE_LOG 대체 매크로
#define UE_LOG(fmt, ...)

UWorldOutlinerWidget::UWorldOutlinerWidget()
    : UWidget("World Outliner")
    , UIManager(&UUIManager::GetInstance())
    , SelectionManager(&USelectionManager::GetInstance())
{
}

UWorldOutlinerWidget::~UWorldOutlinerWidget()
{
    ClearActorTree();
}

void UWorldOutlinerWidget::Initialize()
{
    UIManager = &UUIManager::GetInstance();
    SelectionManager = &USelectionManager::GetInstance();
}

void UWorldOutlinerWidget::Update()
{
    // Refresh actor tree if world changed
    RefreshActorTree();
    
    // Sync selection from viewport
    SyncSelectionFromViewport();
}

void UWorldOutlinerWidget::RenderWidget()
{
    ImGui::Text("World Outliner");
    ImGui::Spacing();
    
    // Toolbar
    RenderToolbar();
    ImGui::Separator();
    
    // Search bar
    RenderSearchBar();
    ImGui::Spacing();
    
    // World status
    UWorld* World = GetCurrentWorld();
    if (!World)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "No World Available");
        return;
    }
    
    ImGui::Text("Objects: %zu", World->GetActors().size());
    ImGui::Separator();
    
    // Actor tree view
    ImGui::BeginChild("ActorTreeView", ImVec2(0, -30), true);
    
    for (auto* RootNode : RootNodes)
    {
        if (RootNode && ShouldShowActor(RootNode->Actor))
        {
            RenderActorNode(RootNode);
        }
    }
    
    ImGui::EndChild();
    
    // Context menu
    if (bShowContextMenu)
    {
        RenderContextMenu();
    }
    
    // Status bar
    ImGui::Separator();
    AActor* SelectedActor = SelectionManager->GetSelectedActor();
    if (SelectedActor)
    {
        ImGui::Text("Selected: %s", SelectedActor->GetName().c_str());
    }
    else
    {
        ImGui::Text("No selection");
    }
}

UWorld* UWorldOutlinerWidget::GetCurrentWorld() const
{
    if (!UIManager)
        return nullptr;
    return UIManager->GetWorld();
}

void UWorldOutlinerWidget::RefreshActorTree()
{
    UWorld* World = GetCurrentWorld();
    if (!World)
    {
        ClearActorTree();
        return;
    }
    
    // Clear existing tree
    ClearActorTree();
    
    // Build new hierarchy
    BuildActorHierarchy();
}

void UWorldOutlinerWidget::BuildActorHierarchy()
{
    UWorld* World = GetCurrentWorld();
    if (!World)
        return;
    
    const TArray<AActor*>& Actors = World->GetActors();
    
    // For now, create flat hierarchy (TODO: implement parent-child relationships)
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            FActorTreeNode* Node = new FActorTreeNode(Actor);
            RootNodes.push_back(Node);
        }
    }
}

void UWorldOutlinerWidget::RenderActorNode(FActorTreeNode* Node, int32 Depth)
{
    if (!Node || !Node->Actor)
        return;
    
    AActor* Actor = Node->Actor;
    
    // Skip if doesn't pass filters
    if (!ShouldShowActor(Actor))
        return;
    
    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    
    // Check if selected
    bool bIsSelected = SelectionManager->IsActorSelected(Actor);
    if (bIsSelected)
    {
        NodeFlags |= ImGuiTreeNodeFlags_Selected;
    }
    
    // Leaf node if no children
    if (Node->Children.empty())
    {
        NodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    
    // Create unique ID for ImGui
    ImGui::PushID(Actor);
    
    // Visibility toggle button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    const char* VisibilityIcon = Node->bIsVisible ? "👁" : "🚫";
    if (ImGui::SmallButton(VisibilityIcon))
    {
        HandleActorVisibilityToggle(Actor);
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // Actor name and tree node
    bool bNodeOpen = ImGui::TreeNodeEx(Actor->GetName().c_str(), NodeFlags);
    
    // Handle selection
    if (ImGui::IsItemClicked())
    {
        HandleActorSelection(Actor);
    }
    
    // Handle right-click context menu
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        ContextMenuTarget = Actor;
        bShowContextMenu = true;
        ImGui::OpenPopup("ActorContextMenu");
    }
    
    // Handle drag source
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ACTOR_DRAG", &Actor, sizeof(AActor*));
        ImGui::Text("Move %s", Actor->GetName().c_str());
        DragSource = Actor;
        ImGui::EndDragDropSource();
    }
    
    // Handle drop target
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("ACTOR_DRAG"))
        {
            AActor* DroppedActor = *(AActor**)Payload->Data;
            if (DroppedActor != Actor)
            {
                // TODO: Implement hierarchy reparenting
                UE_LOG("Would reparent %s to %s", DroppedActor->GetName().c_str(), Actor->GetName().c_str());
            }
        }
        ImGui::EndDragDropTarget();
    }
    
    // Render children if node is open
    if (bNodeOpen && !Node->Children.empty())
    {
        for (FActorTreeNode* Child : Node->Children)
        {
            RenderActorNode(Child, Depth + 1);
        }
    }
    
    if (bNodeOpen && !Node->Children.empty())
    {
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

bool UWorldOutlinerWidget::ShouldShowActor(AActor* Actor) const
{
    if (!Actor)
        return false;
    
    // Filter by selection
    if (bShowOnlySelectedObjects && !SelectionManager->IsActorSelected(Actor))
        return false;
    
    // Filter by search
    if (!PassesSearchFilter(Actor))
        return false;
    
    return true;
}

void UWorldOutlinerWidget::HandleActorSelection(AActor* Actor)
{
    if (!Actor || !SelectionManager)
        return;
    
    // Clear previous selection and select this actor
    SelectionManager->ClearSelection();
    SelectionManager->SelectActor(Actor);
    
    UE_LOG("WorldOutliner: Selected actor %s", Actor->GetName().c_str());
}

void UWorldOutlinerWidget::HandleActorVisibilityToggle(AActor* Actor)
{
    if (!Actor)
        return;
    
    // Find the node and toggle visibility
    FActorTreeNode* Node = FindNodeByActor(Actor);
    if (Node)
    {
        Node->bIsVisible = !Node->bIsVisible;
        UE_LOG("WorldOutliner: Toggled visibility for %s: %s", 
               Actor->GetName().c_str(), Node->bIsVisible ? "Visible" : "Hidden");
    }
}

void UWorldOutlinerWidget::HandleActorRename(AActor* Actor)
{
    // TODO: Implement inline renaming
    UE_LOG("WorldOutliner: Rename not implemented yet");
}

void UWorldOutlinerWidget::HandleActorDelete(AActor* Actor)
{
    if (!Actor)
        return;
    
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        World->DestroyActor(Actor);
        UE_LOG("WorldOutliner: Deleted actor %s", Actor->GetName().c_str());
    }
}

void UWorldOutlinerWidget::HandleActorDuplicate(AActor* Actor)
{
    // TODO: Implement actor duplication
    UE_LOG("WorldOutliner: Duplicate not implemented yet");
}

void UWorldOutlinerWidget::RenderContextMenu()
{
    if (ImGui::BeginPopup("ActorContextMenu"))
    {
        if (ContextMenuTarget)
        {
            ImGui::Text("Actor: %s", ContextMenuTarget->GetName().c_str());
            ImGui::Separator();
            
            if (ImGui::MenuItem("Rename"))
            {
                HandleActorRename(ContextMenuTarget);
            }
            
            if (ImGui::MenuItem("Duplicate"))
            {
                HandleActorDuplicate(ContextMenuTarget);
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Delete"))
            {
                HandleActorDelete(ContextMenuTarget);
            }
        }
        
        ImGui::EndPopup();
    }
    else
    {
        bShowContextMenu = false;
        ContextMenuTarget = nullptr;
    }
}

void UWorldOutlinerWidget::RenderSearchBar()
{
    ImGui::Text("Search:");
    ImGui::SameLine();
    
    char SearchBuffer[256];
    strcpy_s(SearchBuffer, SearchFilter.c_str());
    
    if (ImGui::InputText("##SearchFilter", SearchBuffer, sizeof(SearchBuffer)))
    {
        SearchFilter = SearchBuffer;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        SearchFilter = "";
    }
}

bool UWorldOutlinerWidget::PassesSearchFilter(AActor* Actor) const
{
    if (SearchFilter.empty())
        return true;
    
    if (!Actor)
        return false;
    
    // Case-insensitive search in actor name
    std::string ActorName = Actor->GetName();
    std::string Filter = SearchFilter;
    
    std::transform(ActorName.begin(), ActorName.end(), ActorName.begin(), ::tolower);
    std::transform(Filter.begin(), Filter.end(), Filter.begin(), ::tolower);
    
    return ActorName.find(Filter) != std::string::npos;
}

void UWorldOutlinerWidget::RenderToolbar()
{
    // Filter toggles
    ImGui::Checkbox("Selected Only", &bShowOnlySelectedObjects);
    ImGui::SameLine();
    ImGui::Checkbox("Show Hidden", &bShowHiddenObjects);
    
    ImGui::SameLine();
    if (ImGui::Button("Refresh"))
    {
        RefreshActorTree();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Expand All"))
    {
        // TODO: Expand all nodes
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Collapse All"))
    {
        // TODO: Collapse all nodes
    }
}

void UWorldOutlinerWidget::ClearActorTree()
{
    for (auto* Node : RootNodes)
    {
        delete Node;
    }
    RootNodes.clear();
}

UWorldOutlinerWidget::FActorTreeNode* UWorldOutlinerWidget::FindNodeByActor(AActor* Actor)
{
    if (!Actor)
        return nullptr;
    
    // Simple linear search for now (TODO: optimize with hash map)
    for (FActorTreeNode* Node : RootNodes)
    {
        if (Node && Node->Actor == Actor)
            return Node;
    }
    
    return nullptr;
}

void UWorldOutlinerWidget::SyncSelectionFromViewport()
{
    // This would be called to sync selection from 3D viewport to outliner
    // Currently the selection system is already centralized via SelectionManager
}

void UWorldOutlinerWidget::SyncSelectionToViewport(AActor* Actor)
{
    // Selection is already handled via SelectionManager
    // The 3D viewport will automatically respond to selection changes
}