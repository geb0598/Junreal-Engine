#include "pch.h"
#include "WorldOutlinerWidget.h"
#include "../UIManager.h"
#include "../../ImGui/imgui.h"
#include "../../World.h"
#include "../../Actor.h"
#include "../../StaticMeshActor.h"
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
    // Check if we need to refresh (world changed or actors added/removed)
    static size_t LastActorCount = 0;
    
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        size_t CurrentActorCount = World->GetActors().size();
        if (CurrentActorCount != LastActorCount)
        {
            RefreshActorTree();
            LastActorCount = CurrentActorCount;
        }
    }
    else if (LastActorCount != 0)
    {
        RefreshActorTree();
        LastActorCount = 0;
    }
    
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
        if (RootNode)
        {
            // Categories are always shown, individual actors are filtered
            if (RootNode->IsCategory() || ShouldShowActor(RootNode->Actor))
            {
                RenderActorNode(RootNode);
            }
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
    
    // Build categorized hierarchy instead of flat
    BuildCategorizedHierarchy();
}

void UWorldOutlinerWidget::RenderActorNode(FActorTreeNode* Node, int32 Depth)
{
    if (!Node)
        return;
    
    // Handle category nodes
    if (Node->IsCategory())
    {
        RenderCategoryNode(Node, Depth);
        return;
    }
    
    // Handle actor nodes
    if (!Node->Actor)
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
    
    // Sync node visibility with actual actor state each frame
    Node->bIsVisible = Actor->IsActorVisible();
    
    // Visibility toggle button (only for actors)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    const char* VisibilityIcon = Node->bIsVisible ? "O" : "X";
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
    
    // Sync with UIManager for gizmo positioning
    if (UIManager)
    {
        UIManager->SetPickedActor(Actor);
        
        // If there's a gizmo actor, position it at the selected actor
        if (UIManager->GetGizmoActor() && Actor)
        {
            UIManager->GetGizmoActor()->SetActorLocation(Actor->GetActorLocation());
        }
    }
    
    UE_LOG("WorldOutliner: Selected actor %s", Actor->GetName().c_str());
}

void UWorldOutlinerWidget::HandleActorVisibilityToggle(AActor* Actor)
{
    if (!Actor)
        return;
    
    // Toggle the actor's actual visibility state
    bool bNewVisible = Actor->GetActorHiddenInGame(); // If hidden, make visible
    Actor->SetActorHiddenInGame(!bNewVisible);
    
    // Update the node to match the actor's state
    FActorTreeNode* Node = FindNodeByActor(Actor);
    if (Node)
    {
        Node->bIsVisible = Actor->IsActorVisible();
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
            const char* ClassName = "Unknown";
            if (ContextMenuTarget->GetClass())
            {
                ClassName = ContextMenuTarget->GetClass()->Name;
            }
            ImGui::Text("Type: %s", ClassName);
            ImGui::Separator();
            
            // Selection actions
            if (ImGui::MenuItem("Focus in Viewport"))
            {
                HandleActorSelection(ContextMenuTarget);
            }
            
            ImGui::Separator();
            
            // Transform actions
            if (ImGui::MenuItem("Reset Transform"))
            {
                if (ContextMenuTarget)
                {
                    ContextMenuTarget->SetActorLocation(FVector(0, 0, 0));
                    ContextMenuTarget->SetActorRotation(FQuat::Identity());
                    ContextMenuTarget->SetActorScale(FVector(1, 1, 1));
                }
            }
            
            if (ImGui::MenuItem("Reset Position"))
            {
                if (ContextMenuTarget)
                {
                    ContextMenuTarget->SetActorLocation(FVector(0, 0, 0));
                }
            }
            
            ImGui::Separator();
            
            // Edit actions
            if (ImGui::MenuItem("Rename"))
            {
                HandleActorRename(ContextMenuTarget);
            }
            
            if (ImGui::MenuItem("Duplicate"))
            {
                HandleActorDuplicate(ContextMenuTarget);
            }
            
            ImGui::Separator();
            
            // Danger zone
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            if (ImGui::MenuItem("Delete"))
            {
                HandleActorDelete(ContextMenuTarget);
            }
            ImGui::PopStyleColor();
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
        ExpandAllCategories();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Collapse All"))
    {
        CollapseAllCategories();
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
    
    // Search through categories and their children
    for (FActorTreeNode* RootNode : RootNodes)
    {
        if (!RootNode)
            continue;
            
        // Check if this root node is the actor (for backward compatibility)
        if (RootNode->IsActor() && RootNode->Actor == Actor)
            return RootNode;
            
        // Search within category children
        if (RootNode->IsCategory())
        {
            for (FActorTreeNode* Child : RootNode->Children)
            {
                if (Child && Child->IsActor() && Child->Actor == Actor)
                    return Child;
            }
        }
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

// Category Management Implementation
FString UWorldOutlinerWidget::GetActorCategory(AActor* Actor) const
{
    if (!Actor)
        return "Unknown";
    
    FString ActorName = Actor->GetName();
    
    // Extract category from actor name (assumes format: "Type_Number")
    size_t UnderscorePos = ActorName.find('_');
    if (UnderscorePos != std::string::npos)
    {
        return ActorName.substr(0, UnderscorePos);
    }
    
    // Fallback: use the full name as category if no underscore found
    return ActorName;
}

UWorldOutlinerWidget::FActorTreeNode* UWorldOutlinerWidget::FindOrCreateCategoryNode(const FString& CategoryName)
{
    // Look for existing category node in root nodes
    for (FActorTreeNode* Node : RootNodes)
    {
        if (Node && Node->IsCategory() && Node->CategoryName == CategoryName)
        {
            return Node;
        }
    }
    
    // Create new category node if not found
    FActorTreeNode* CategoryNode = new FActorTreeNode(CategoryName);
    RootNodes.push_back(CategoryNode);
    return CategoryNode;
}

void UWorldOutlinerWidget::BuildCategorizedHierarchy()
{
    UWorld* World = GetCurrentWorld();
    if (!World)
        return;
    
    const TArray<AActor*>& Actors = World->GetActors();
    
    // Group actors by category
    for (AActor* Actor : Actors)
    {
        if (!Actor)
            continue;
            
        FString CategoryName = GetActorCategory(Actor);
        FActorTreeNode* CategoryNode = FindOrCreateCategoryNode(CategoryName);
        
        // Create actor node and add to category
        FActorTreeNode* ActorNode = new FActorTreeNode(Actor);
        ActorNode->Parent = CategoryNode;
        // Initialize node visibility from actor's actual visibility state
        ActorNode->bIsVisible = Actor->IsActorVisible();
        CategoryNode->Children.push_back(ActorNode);
    }
    
    // Initialize category visibility based on child actors
    for (FActorTreeNode* CategoryNode : RootNodes)
    {
        if (CategoryNode && CategoryNode->IsCategory())
        {
            // Category is visible if any child is visible
            bool bAnyCategoryChildVisible = false;
            for (FActorTreeNode* Child : CategoryNode->Children)
            {
                if (Child && Child->bIsVisible)
                {
                    bAnyCategoryChildVisible = true;
                    break;
                }
            }
            CategoryNode->bIsVisible = bAnyCategoryChildVisible;
        }
    }
}

void UWorldOutlinerWidget::HandleCategorySelection(FActorTreeNode* CategoryNode)
{
    if (!CategoryNode || !CategoryNode->IsCategory())
        return;
        
    // Toggle category expansion
    CategoryNode->bIsExpanded = !CategoryNode->bIsExpanded;
    
    UE_LOG("WorldOutliner: Toggled category %s: %s", 
           CategoryNode->CategoryName.c_str(), 
           CategoryNode->bIsExpanded ? "Expanded" : "Collapsed");
}

void UWorldOutlinerWidget::HandleCategoryVisibilityToggle(FActorTreeNode* CategoryNode)
{
    if (!CategoryNode || !CategoryNode->IsCategory())
        return;
        
    // Toggle category visibility
    CategoryNode->bIsVisible = !CategoryNode->bIsVisible;
    
    // Apply visibility to all child actors
    for (FActorTreeNode* Child : CategoryNode->Children)
    {
        if (Child && Child->IsActor() && Child->Actor)
        {
            Child->Actor->SetActorHiddenInGame(!CategoryNode->bIsVisible);
            Child->bIsVisible = CategoryNode->bIsVisible;
        }
    }
    
    UE_LOG("WorldOutliner: Toggled category visibility %s: %s", 
           CategoryNode->CategoryName.c_str(), 
           CategoryNode->bIsVisible ? "Visible" : "Hidden");
}

void UWorldOutlinerWidget::RenderCategoryNode(FActorTreeNode* CategoryNode, int32 Depth)
{
    if (!CategoryNode || !CategoryNode->IsCategory())
        return;
        
    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    
    // Categories are always expandable
    if (!CategoryNode->Children.empty())
    {
        if (CategoryNode->bIsExpanded)
        {
            NodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
    }
    else
    {
        // Empty category - show as leaf
        NodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    
    // Create unique ID for ImGui using category name
    ImGui::PushID(CategoryNode->CategoryName.c_str());
    
    // Category visibility toggle button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    const char* VisibilityIcon = CategoryNode->bIsVisible ? "O" : "X";
    if (ImGui::SmallButton(VisibilityIcon))
    {
        HandleCategoryVisibilityToggle(CategoryNode);
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // Category name with object count
    FString DisplayText = CategoryNode->CategoryName + " (" + std::to_string(CategoryNode->Children.size()) + ")";
    bool bNodeOpen = ImGui::TreeNodeEx(DisplayText.c_str(), NodeFlags);
    
    // Handle category click
    if (ImGui::IsItemClicked())
    {
        HandleCategorySelection(CategoryNode);
    }
    
    // Update expansion state based on ImGui tree state
    CategoryNode->bIsExpanded = bNodeOpen;
    
    // Render child actors if category is expanded
    if (bNodeOpen && !CategoryNode->Children.empty())
    {
        for (FActorTreeNode* Child : CategoryNode->Children)
        {
            RenderActorNode(Child, Depth + 1);
        }
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

void UWorldOutlinerWidget::ExpandAllCategories()
{
    for (FActorTreeNode* RootNode : RootNodes)
    {
        if (RootNode && RootNode->IsCategory())
        {
            RootNode->bIsExpanded = true;
        }
    }
    UE_LOG("WorldOutliner: Expanded all categories");
}

void UWorldOutlinerWidget::CollapseAllCategories()
{
    for (FActorTreeNode* RootNode : RootNodes)
    {
        if (RootNode && RootNode->IsCategory())
        {
            RootNode->bIsExpanded = false;
        }
    }
    UE_LOG("WorldOutliner: Collapsed all categories");
}

