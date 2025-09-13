#include "ObjectFactory.h"   // namespace ObjectFactory, IMPLEMENT_CLASS 매크로
#include "ActorComponent.h"
#include"CameraActor.h"
#include"CameraComponent.h"
#include "InputManager.h"
#include "World.h"
#include "MemoryManager.h"
#include "Material.h"
#include "MeshComponent.h"
#include "PrimitiveComponent.h"
#include "ResourceManager.h"
#include "SceneComponent.h"
#include "SceneLoader.h"
#include "SelectionManager.h"
#include "StaticMesh.h"
#include "StaticMeshActor.h"
#include "StaticMeshComponent.h"
#include "UI/UIManager.h"
#include "Vector.h"
#include"GizmoRotateComponent.h"
#include"GizmoScaleComponent.h"
#include "Object.h"
#include "Actor.h"

#include "UI/GlobalConsole.h"
#include "UI/UIManager.h"
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
#include "UI/Widget/Widget.h"
#include "UI/Window/ConsoleWindow.h"
#include "UI/Window/ControlPanelWindow.h"
#include "UI/Window/ExperimentalFeatureWindow.h"
#include "UI/Window/OutlinerWindow.h"
#include "UI/Window/UIWindow.h"

// … 추가 클래스 헤더들 계속 include

// 여기서 한 번에 등록
IMPLEMENT_CLASS(UWorld)


IMPLEMENT_CLASS(UPrimitiveComponent)
IMPLEMENT_CLASS(UActorComponent)
IMPLEMENT_CLASS(USceneComponent)
IMPLEMENT_CLASS(UCameraComponent)
IMPLEMENT_CLASS(UStaticMeshComponent)

IMPLEMENT_CLASS(AActor)
IMPLEMENT_CLASS(ACameraActor)
IMPLEMENT_CLASS(AStaticMeshActor)
IMPLEMENT_CLASS(AGizmoActor)
IMPLEMENT_CLASS(AGridActor)

IMPLEMENT_CLASS(UStaticMesh)

IMPLEMENT_CLASS(UGizmoArrowComponent)
IMPLEMENT_CLASS(UGizmoScaleComponent)
IMPLEMENT_CLASS(UGizmoRotateComponent)

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

IMPLEMENT_CLASS(UUIWindow)
IMPLEMENT_CLASS(UConsoleWindow)
IMPLEMENT_CLASS(UControlPanelWindow)
IMPLEMENT_CLASS(UExperimentalFeatureWindow)
IMPLEMENT_CLASS(UOutlinerWindow)

IMPLEMENT_CLASS(UGlobalConsole)


//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)


// … 필요한 만큼 나열
