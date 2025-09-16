#include "pch.h"
#include "PrimitiveSpawnWidget.h"
#include "../UIManager.h"
#include "../../ImGui/imgui.h"
#include "../../World.h"
#include "../../StaticMeshActor.h"
#include "../../Vector.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

// UE_LOG 대체 매크로
#define UE_LOG(fmt, ...)

// std 함수들 정의
using std::max;
using std::min;
using std::to_string;

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
	case 0: return "Cube";
	case 1: return "Sphere"; 
	case 2: return "Triangle";
	case 3: return "Arrow";
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

	// Primitive 타입 선택
	const char* PrimitiveTypes[] = {
		"Cube",
		"Sphere", 
		"Triangle",
		"Arrow"
	};

	ImGui::Text("Primitive Type:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120);
	ImGui::Combo("##PrimitiveType", &SelectedPrimitiveType, PrimitiveTypes, 4);

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
		SpawnActors();
	}

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
		SpawnActors();
	}
	ImGui::SameLine();
	if (ImGui::Button("Spawn 5 Random"))
	{
		NumberOfSpawn = 5;
		SpawnActors();
	}
}

void UPrimitiveSpawnWidget::SpawnActors() const
{
	UWorld* World = GetCurrentWorld();
	if (!World)
	{
		UE_LOG("PrimitiveSpawn: No World available for spawning");
		return;
	}

	UE_LOG("PrimitiveSpawn: Spawning %d %s actors", NumberOfSpawn, GetPrimitiveTypeName(SelectedPrimitiveType));

	int32 SuccessCount = 0;
	
	// 지정된 개수만큼 액터 생성
	for (int32 i = 0; i < NumberOfSpawn; i++)
	{
		// 기본 트랜스폼 생성
		FVector SpawnLocation = GenerateRandomLocation();
		FQuat SpawnRotation = GenerateRandomRotation();
		float SpawnScale = GenerateRandomScale();
		FVector SpawnScaleVec(SpawnScale, SpawnScale, SpawnScale);
		
		FTransform SpawnTransform(SpawnLocation, SpawnRotation, SpawnScaleVec);

		// StaticMeshActor 생성 (실제 프리미티브 타입에 따른 메시 설정)
		AStaticMeshActor* NewActor = World->SpawnActor<AStaticMeshActor>(SpawnTransform);
		
		if (NewActor)
		{
			// 선택된 프리미티브 타입에 따라 메시 설정
			FString MeshPath;
			switch (SelectedPrimitiveType)
			{
			case 0: MeshPath = "Cube.obj"; break;
			case 1: MeshPath = "Sphere.obj"; break; 
			case 2: MeshPath = "Triangle.obj"; break;
			default: MeshPath = "Cube.obj"; break;
			}
			
			// StaticMeshComponent에 메시 설정 (API가 있다면)
			if (auto* MeshComp = Cast<UMeshComponent>(NewActor->GetStaticMeshComponent()))
			{
				MeshComp->SetMeshResource(MeshPath);
				MeshComp->SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
				if (MeshPath == "Sphere.obj")
				{
					Cast<AStaticMeshActor>(NewActor)->SetCollisionComponent(EPrimitiveType::Sphere);
					//컬리젼 컴포넌트의 메쉬 정보를 강제로 세팅 
				}
				else
				{
					Cast<AStaticMeshActor>(NewActor)->SetCollisionComponent();
				}
			}
			
			// 액터 이름 설정 - World에서 unique name 생성
			FString ActorName = World->GenerateUniqueActorName(GetPrimitiveTypeName(SelectedPrimitiveType));
			NewActor->SetName(ActorName);
			
			SuccessCount++;
			
			UE_LOG("PrimitiveSpawn: Created %s at (%.2f, %.2f, %.2f) with scale %.2f", 
			       GetPrimitiveTypeName(SelectedPrimitiveType),
			       SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z, SpawnScale);
		}
		else
		{
			UE_LOG("PrimitiveSpawn: Failed to spawn actor %d", i);
		}
	}

	UE_LOG("PrimitiveSpawn: Successfully spawned %d/%d actors", SuccessCount, NumberOfSpawn);
}
