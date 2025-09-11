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
#include "UIManager.h"
#include "Vector.h"
#include"GizmoRotateComponent.h"
#include"GizmoScaleComponent.h"
#include "Object.h"
#include "Actor.h"
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

    


//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)
//IMPLEMENT_CLASS(AActor)


// … 필요한 만큼 나열
