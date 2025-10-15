﻿#include "pch.h"

#include "MemoryManager.h"
#include "MeshComponent.h"
#include "SceneLoader.h"
#include "SelectionManager.h"
#include "GizmoRotateComponent.h"
#include "GizmoScaleComponent.h"
#include "CameraComponent.h"
#include "CameraActor.h"
#include "StaticMeshActor.h"
#include "MeshLoader.h"
#include "LineComponent.h"
#include "Line.h"

#include "UI/Factory/UIWindowFactory.h"
#include "UI/ImGui/ImGuiHelper.h"
#include "UI/Widget/ActorTerminationWidget.h"
#include "UI/Widget/CameraControlWidget.h"
#include "UI/Widget/ConsoleWidget.h"
#include "UI/Widget/FPSWidget.h"
#include "UI/Widget/InputInformationWidget.h"
#include "UI/Widget/PrimitiveSpawnWidget.h"
#include "UI/Widget/SceneIOWidget.h"
#include "UI/Widget/TargetActorTransformWidget.h"
#include "UI/Widget/SceneManagerWidget.h"
#include "UI/Widget/Widget.h"
#include "UI/Window/ConsoleWindow.h"
#include "UI/Window/ControlPanelWindow.h"
#include "UI/Window/PropertyWindow.h"
#include "UI/Window/ExperimentalFeatureWindow.h"
#include "UI/Window/SceneWindow.h"
#include "UI/Window/UIWindow.h"
#include "TextRenderComponent.h"
#include "StaticMesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "DynamicMesh.h"
#include "LineDynamicMesh.h"
#include "TextQuad.h"
#include "RenderViewportSwitcherWidget.h"
#include "MenuBarWidget.h"
#include "Octree.h"
#include "Engine.h"
#include "EditorEngine.h"
#include "GameEngine.h"
#include "Level.h"
#include "DecalComponent.h"
#include "DecalActor.h"
#include "SpotLightActor.h"
#include "ExponentialHeightFog.h"
#include "FireballComponent.h"
#include "MeshComponent.h"
#include "RotationMovementComponent.h"
#include "ProjectileMovementComponent.h"
#include "ExponentialHeightFogComponent.h"
// ... (rest of the file)
IMPLEMENT_CLASS(UWorld)


//IMPLEMENT_CLASS(UPrimitiveComponent)
IMPLEMENT_CLASS(UActorComponent)
IMPLEMENT_CLASS(USceneComponent)
IMPLEMENT_CLASS(UCameraComponent)
IMPLEMENT_CLASS(UStaticMeshComponent)
IMPLEMENT_CLASS(UMovementComponent)
IMPLEMENT_CLASS(URotationMovementComponent)
IMPLEMENT_CLASS(UProjectileMovementComponent)
//IMPLEMENT_CLASS(UShapeComponent)

IMPLEMENT_CLASS(AActor)
IMPLEMENT_CLASS(ACameraActor)
IMPLEMENT_CLASS(AStaticMeshActor)
IMPLEMENT_CLASS(AGizmoActor)
IMPLEMENT_CLASS(AGridActor)
IMPLEMENT_CLASS(ADecalActor)
IMPLEMENT_CLASS(ASpotLightActor)
IMPLEMENT_CLASS(AExponentialHeightFog)

IMPLEMENT_CLASS(UGizmoArrowComponent)
IMPLEMENT_CLASS(UGizmoScaleComponent)
IMPLEMENT_CLASS(UGizmoRotateComponent)
IMPLEMENT_CLASS(ULineComponent)
IMPLEMENT_CLASS(ULine)
IMPLEMENT_CLASS(UTextRenderComponent)
IMPLEMENT_CLASS(UBillboardComponent)
IMPLEMENT_CLASS(UDecalComponent)
IMPLEMENT_CLASS(UFireBallComponent)
IMPLEMENT_CLASS(UExponentialHeightFogComponent)

// Managers / Systems
IMPLEMENT_CLASS(UInputManager)
IMPLEMENT_CLASS(UUIManager)
IMPLEMENT_CLASS(USelectionManager)
IMPLEMENT_CLASS(UMeshLoader)
IMPLEMENT_CLASS(UResourceManager)

//UI Class
IMPLEMENT_CLASS(UUIWindowFactory)
IMPLEMENT_CLASS(UImGuiHelper)

IMPLEMENT_CLASS(UWidget)
IMPLEMENT_CLASS(UActorTerminationWidget)
IMPLEMENT_CLASS(UCameraControlWidget)
IMPLEMENT_CLASS(UConsoleWidget)
IMPLEMENT_CLASS(UFPSWidget)
IMPLEMENT_CLASS(UInputInformationWidget)
IMPLEMENT_CLASS(UPrimitiveSpawnWidget)
IMPLEMENT_CLASS(USceneIOWidget)
IMPLEMENT_CLASS(UTargetActorTransformWidget)
IMPLEMENT_CLASS(USceneManagerWidget)

IMPLEMENT_CLASS(UUIWindow)
IMPLEMENT_CLASS(UConsoleWindow)
IMPLEMENT_CLASS(UControlPanelWindow)
IMPLEMENT_CLASS(UPropertyWindow)
IMPLEMENT_CLASS(UExperimentalFeatureWindow)
IMPLEMENT_CLASS(USceneWindow)
IMPLEMENT_CLASS(URenderViewportSwitcherWidget)
IMPLEMENT_CLASS(UMenuBarWidget)
//IMPLEMENT_CLASS(UMultiViewportWindow)

IMPLEMENT_CLASS(UGlobalConsole)

IMPLEMENT_CLASS(UMaterial)
IMPLEMENT_CLASS(UResourceBase)
IMPLEMENT_CLASS(UStaticMesh)
IMPLEMENT_CLASS(UShader)
IMPLEMENT_CLASS(UTexture)
IMPLEMENT_CLASS(UDynamicMesh)
IMPLEMENT_CLASS(ULineDynamicMesh)
IMPLEMENT_CLASS(UTextQuad)
//IMPLEMENT_CLASS(UOctree)




/*================Editor====================*/
IMPLEMENT_CLASS(UEngine)
IMPLEMENT_CLASS(UGameEngine)
IMPLEMENT_CLASS(UEditorEngine)

IMPLEMENT_CLASS(ULevel)