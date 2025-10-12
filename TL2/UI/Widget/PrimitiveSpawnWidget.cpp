﻿#include "pch.h"
#include "PrimitiveSpawnWidget.h"
#include "../UIManager.h"
#include "../../ImGui/imgui.h"
#include "../../World.h"
#include "../../StaticMeshActor.h"
#include "../../DecalActor.h"
#include "../../SpotLightActor.h"
#include "../../Vector.h"
#include "ObjManager.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include "ObjectIterator.h"

//// UE_LOG 대체 매크로
//#define UE_LOG(fmt, ...)

// std 함수들 정의
using std::max;
using std::min;
using std::to_string;

static inline FString GetBaseNameNoExt(const FString& Path)
{
	const size_t Sep = Path.find_last_of("/\\");
	const size_t Start = (Sep == FString::npos) ? 0 : Sep + 1;

	const FString Ext = ".obj";
	size_t End = Path.size();
	if (End >= Ext.size() && Path.compare(End - Ext.size(), Ext.size(), Ext) == 0)
	{
		End -= Ext.size();
	}

	if (Start <= End) 
	{
		return Path.substr(Start, End - Start);
	}

	return Path;
}

UPrimitiveSpawnWidget::UPrimitiveSpawnWidget()
	: UWidget("Primitive Spawn Widget")
	, UIManager(&UUIManager::GetInstance())
{
	// 랜덤 시드 초기화
	srand(static_cast<unsigned int>(time(nullptr)));
}

UPrimitiveSpawnWidget::~UPrimitiveSpawnWidget() = default;

void UPrimitiveSpawnWidget::Initialize()
{
	// UIManager 참조 확보
	UIManager = &UUIManager::GetInstance();
}

void UPrimitiveSpawnWidget::Update()
{
	// 필요시 업데이트 로직 추가
}

UWorld* UPrimitiveSpawnWidget::GetCurrentWorld() const
{
	if (!UIManager)
		return nullptr;
		
	return UIManager->GetWorld();
}

const char* UPrimitiveSpawnWidget::GetPrimitiveTypeName(int32 TypeIndex) const
{
	switch (TypeIndex)
	{
	case 0: return "StaticMesh";
	default: return "Unknown";
	}
}

FVector UPrimitiveSpawnWidget::GenerateRandomLocation() const
{
	float RandomX = SpawnRangeMin + (static_cast<float>(rand()) / RAND_MAX) * (SpawnRangeMax - SpawnRangeMin);
	float RandomY = SpawnRangeMin + (static_cast<float>(rand()) / RAND_MAX) * (SpawnRangeMax - SpawnRangeMin);
	float RandomZ = SpawnRangeMin + (static_cast<float>(rand()) / RAND_MAX) * (SpawnRangeMax - SpawnRangeMin);
	
	return FVector(RandomX, RandomY, RandomZ);
}

float UPrimitiveSpawnWidget::GenerateRandomScale() const
{
	if (!bRandomScale)
		return 1.0f;
		
	return MinScale + (static_cast<float>(rand()) / RAND_MAX) * (MaxScale - MinScale);
}

FQuat UPrimitiveSpawnWidget::GenerateRandomRotation() const
{
	if (!bRandomRotation)
		return FQuat::Identity();
	
	// 랜덤 오일러 각도 생성 (도 단위)
	float RandomPitch = (static_cast<float>(rand()) / RAND_MAX) * 360.0f - 180.0f;
	float RandomYaw = (static_cast<float>(rand()) / RAND_MAX) * 360.0f - 180.0f;
	float RandomRoll = (static_cast<float>(rand()) / RAND_MAX) * 360.0f - 180.0f;
	
	return FQuat::MakeFromEuler(FVector(RandomPitch, RandomYaw, RandomRoll));
}

void UPrimitiveSpawnWidget::RenderWidget()
{
    ImGui::Text("Primitive Actor Spawner");
    ImGui::Spacing();

    // Primitive 타입 선택: StaticMesh만 노출
    const char* SpawnTypes[] = { "Static Mesh", "Decal", "Actor", "Spot Light" };
    ImGui::Text("Primitive Type:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::Combo("##Primitive Type", &SelectedPrimitiveType, SpawnTypes, IM_ARRAYSIZE(SpawnTypes));

    switch (SelectedPrimitiveType)
    {
    case 0:
    {
        auto& ResourceManager = UResourceManager::GetInstance();

        // 최신 목록 갱신 (캐시 보관)
        CachedMeshFilePaths = ResourceManager.GetAllStaticMeshFilePaths();

        // 표시용 이름(파일명 스템)과 ImGui 아이템 배열 구성
        TArray<FString> DisplayNames;
        DisplayNames.reserve(CachedMeshFilePaths.size());
        for (const FString& Path : CachedMeshFilePaths)
        {
            DisplayNames.push_back(GetBaseNameNoExt(Path));
        }

        TArray<const char*> Items;
        Items.reserve(DisplayNames.size());
        for (const FString& Name : DisplayNames)
        {
            Items.push_back(Name.c_str());
        }

        // 기본 선택: Cube가 있으면 자동 선택
        if (SelectedMeshIndex == -1)
        {
            for (int32 i = 0; i < static_cast<int32>(CachedMeshFilePaths.size()); ++i)
            {
                if (GetBaseNameNoExt(CachedMeshFilePaths[i]) == "Cube" ||
                    CachedMeshFilePaths[i] == "Data/Cube.obj")
                {
                    SelectedMeshIndex = i;
                    break;
                }
            }
        }

        ImGui::Text("Static Mesh:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(220);
        ImGui::Combo("##StaticMeshList", &SelectedMeshIndex, Items.data(), static_cast<int>(Items.size()));
        ImGui::SameLine();
        if (ImGui::Button("Clear##StaticMesh"))
        {
            SelectedMeshIndex = -1;
        }

        // Optional: 디버그용 트리
        if (ImGui::TreeNode("Registered Static Meshes"))
        {
            for (const FString& Path : CachedMeshFilePaths)
                ImGui::BulletText("%s", Path.c_str());
            ImGui::TreePop();
        }
        break;
    }
    case 1:
    {
        ImGui::Text("Decal does not require additional resources to spawn.");
        break;
    }
    case 2:
    {
        ImGui::Text("Actor does not require additional resources to spawn.");
        break;
    }
    case 3:
    {
        ImGui::Text("SpotLight does not require additional resources to spawn.");
        break;
    }
    default:
        break;
    }

    // 스폰 개수 설정
    ImGui::Text("Number of Spawn:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##NumberOfSpawn", &NumberOfSpawn);
    NumberOfSpawn = max(1, min(100, NumberOfSpawn));

    // 스폰 버튼
    ImGui::SameLine();
    if (ImGui::Button("Spawn Actors"))
    {
        SpawnActors(SelectedPrimitiveType);
    }

	////Obj Parser 테스트용
	//static std::string fileName;  // 입력값 저장용
	//// 입력창
	//char buffer[256];
	//strncpy_s(buffer, fileName.c_str(), sizeof(buffer));
	//buffer[sizeof(buffer) - 1] = '\0';

	//if (ImGui::InputText("file name", buffer, sizeof(buffer))) {
	//	fileName = buffer;  // std::string으로 갱신
	//}
	//// 버튼
	//if (ImGui::Button("Spawn Dice Test")) {
	//	FObjManager::LoadObjStaticMesh("spaceCompound.obj");
	//}

	ImGui::Spacing();
	ImGui::Separator();

    // 스폰 범위 설정
    ImGui::Text("Spawn Settings");

    ImGui::Text("Position Range:");
    ImGui::SetNextItemWidth(100);
    ImGui::DragFloat("Min##SpawnRange", &SpawnRangeMin, 0.1f, -50.0f, SpawnRangeMax - 0.1f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::DragFloat("Max##SpawnRange", &SpawnRangeMax, 0.1f, SpawnRangeMin + 0.1f, 50.0f);

    // 랜덤 회전 옵션
    ImGui::Checkbox("Random Rotation", &bRandomRotation);

    // 랜덤 스케일 옵션
    ImGui::Checkbox("Random Scale", &bRandomScale);

    if (bRandomScale)
    {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("Min##Scale", &MinScale, 0.01f, 0.1f, MaxScale - 0.01f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("Max##Scale", &MaxScale, 0.01f, MinScale + 0.01f, 10.0f);
    }

    ImGui::Spacing();
    ImGui::Separator();

    // 월드 상태 정보
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        ImGui::Text("World Status: Connected");
        ImGui::Text("Current Actors: %zu", World->GetActors().size());
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "World Status: Not Available");
    }

    ImGui::Spacing();

    // 빠른 스폰 버튼들
    ImGui::Text("Quick Spawn:");
    if (ImGui::Button("Spawn 1 Cube"))
    {
        SelectedPrimitiveType = 0;
        NumberOfSpawn = 1;
        // 기본 선택을 Cube로 강제
        if (!CachedMeshFilePaths.empty())
        {
            SelectedMeshIndex = -1;
            for (int32 i = 0; i < static_cast<int32>(CachedMeshFilePaths.size()); ++i)
            {
                if (GetBaseNameNoExt(CachedMeshFilePaths[i]) == "Cube" ||
                    CachedMeshFilePaths[i] == "Data/Cube.obj")
                {
                    SelectedMeshIndex = i;
                    break;
                }
            }
        }
        SpawnActors(SelectedPrimitiveType);
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn 5 Random"))
    {
        SelectedPrimitiveType = 0;
        NumberOfSpawn = 5;
        SpawnActors(SelectedPrimitiveType);
    }
}

void UPrimitiveSpawnWidget::SpawnActors(int PrimitiveTypeIdx) const
{
    
    UWorld* World = GetCurrentWorld();
    if (!World)
    {
        UE_LOG("PrimitiveSpawn: No World available for spawning");
        return;
    }

    UE_LOG("PrimitiveSpawn: Spawning %d %s actors", NumberOfSpawn, GetPrimitiveTypeName(SelectedPrimitiveType));

    int32 SuccessCount = 0;

    for (int32 i = 0; i < NumberOfSpawn; i++)
    {
        FVector SpawnLocation = FVector(0, 0, 0); //GenerateRandomLocation();
        FQuat   SpawnRotation = FQuat::Identity(); //GenerateRandomRotation();
        float   SpawnScale = 1.0f;//GenerateRandomScale();
        FVector SpawnScaleVec(SpawnScale, SpawnScale, SpawnScale);

        FTransform SpawnTransform(SpawnLocation, SpawnRotation, SpawnScaleVec);

        AActor* SpawnedActor = nullptr;

        switch (PrimitiveTypeIdx)
        {
        case 0:
        {
            AStaticMeshActor* NewMeshActor = World->SpawnActor<AStaticMeshActor>(SpawnTransform);
            if (NewMeshActor)
            {
                // 드롭다운에서 선택한 리소스가 있으면 그걸 사용, 아니면 Cube로 기본 설정
                FString MeshPath = "Data/Cube.obj";
                const bool bHasResourceSelection =
                    (SelectedMeshIndex >= 0) &&
                    (SelectedMeshIndex < static_cast<int32>(CachedMeshFilePaths.size()));

                if (bHasResourceSelection)
                {
                    MeshPath = CachedMeshFilePaths[SelectedMeshIndex];
                }

                if (auto* StaticMeshComp = NewMeshActor->GetStaticMeshComponent())
                {
                    StaticMeshComp->SetStaticMesh(MeshPath);
                }

                FString ActorName = World->GenerateUniqueActorName(
                    bHasResourceSelection ? GetBaseNameNoExt(MeshPath).c_str() : "StaticMesh"
                );
                NewMeshActor->SetName(ActorName);

                SpawnedActor = NewMeshActor;
                
                UE_LOG("PrimitiveSpawn: Created at (%.2f, %.2f, %.2f) scale %.2f using %s",
                    SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z, SpawnScale, MeshPath.c_str());
            }
            break;
        }
        case 1:
        {
            ADecalActor* NewDecalActor = World->SpawnActor<ADecalActor>(SpawnTransform);
            if (NewDecalActor)
            {
                FString ActorName = World->GenerateUniqueActorName("Decal");
                NewDecalActor->SetName(ActorName);

                SpawnedActor = NewDecalActor;

                UE_LOG("PrimitiveSpawn: Created at (%.2f, %.2f, %.2f) scale %.2f",
                    SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z, SpawnScale);
            }
            break;
        }
        case 2:
        {
            AActor* NewActor = World->SpawnActor<AActor>(SpawnTransform);
            if (NewActor)
            {
                FString ActorName = World->GenerateUniqueActorName("Actor");
                NewActor->SetName(ActorName);

                SpawnedActor = NewActor;

                UE_LOG("PrimitiveSpawn: Created empty Actor at (%.2f, %.2f, %.2f)",
                    SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
            }
            break;
        }
        case 3:
        {
            ASpotLightActor* NewSpotLightActor = World->SpawnActor<ASpotLightActor>(SpawnTransform);
            if (NewSpotLightActor)
            {
                FString ActorName = World->GenerateUniqueActorName("SpotLight");
                NewSpotLightActor->SetName(ActorName);

                SpawnedActor = NewSpotLightActor;

                UE_LOG("PrimitiveSpawn: Created SpotLight at (%.2f, %.2f, %.2f)",
                    SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
            }
            break;
        }
        default:
        {
            UE_LOG("PrimitiveSpawn: Invalid PrimitiveTypeIdx: %d", PrimitiveTypeIdx);
            break;
        }
        }
        if (SpawnedActor)
        {
            SuccessCount++;
        }
        else
        {
            UE_LOG("PrimitiveSpawn: Failed to spawn actor %d of type %d", i, PrimitiveTypeIdx);
        }

        UE_LOG("PrimitiveSpawn: Successfully spawned %d/%d actors", SuccessCount, NumberOfSpawn);
    }
}