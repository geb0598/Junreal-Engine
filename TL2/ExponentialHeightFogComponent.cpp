#include "pch.h"
#include "ExponentialHeightFogComponent.h"
#include "MeshLoader.h"

void UExponentialHeightFogComponent::Render(URenderer* Renderer, const FVector& CameraPosition, const FMatrix& View, const FMatrix& Projection, FViewport* Viewport)
{

	//상수버퍼 업데이트
	Renderer->UpdateSetCBuffer(FHeightFogBufferType(
		FogInscatteringColor,
		FogDensity,
		FogHeightFalloff,
		StartDistance,
		FogCutoffDistance,
		FogMaxOpacityDistance,
		FogMaxOpacity,
		//2열에 있던 Z가 Y로 가서 Y를 써야하는 게 아니라 이미 Z가 up으로 변환된 상태(바뀐 기저축상의 점을 월드좌표계로 나타냄)이므로
		//Z값을 써야 이미 바뀐 높이값을 쓸 수 있음(Y는 이미 높이가 아니라 깊이를 나타냄)
		this->GetWorldLocation().Z)); 

	FMatrix ViewProj = View * Projection;

	//Renderer->UpdateSetCBuffer(ViewProjBufferType(View, Projection, CameraPosition));
	Renderer->UpdateSetCBuffer(FViewProjectionInverse(ViewProj.Inverse()));

	Renderer->RenderPostProcessing(UResourceManager::GetInstance().Load<UShader>("HeightFogShader.hlsl"));

	//셰이더 설정
	//Renderer->PrepareShader(UResourceManager::GetInstance().Load<UShader>("HeightFogShader.hlsl"));
	

	//D3D11RHI* RHIDevice = static_cast<D3D11RHI*>(Renderer->GetRHIDevice());
	//ID3D11DeviceContext* DeviceContext = RHIDevice->GetDeviceContext();

	//DeviceContext->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
	//DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	////텍스처 설정
	//ID3D11ShaderResourceView* SRVList[]{ RHIDevice->GetOffScreenSRV(), RHIDevice->GetDepthSRV() };

	//DeviceContext->PSSetShaderResources(0, 2, SRVList);

	//RHIDevice->PSSetDefaultSampler(0);

	//DeviceContext->Draw(3, 0);
	//
	//SRVList[0] = nullptr; SRVList[1] = nullptr;
	//DeviceContext->PSSetShaderResources(0, 2, SRVList);
	//RHIDevice->OMSetRenderTargets(true);
}

UObject* UExponentialHeightFogComponent::Duplicate()
{
	UExponentialHeightFogComponent* DuplicatedComponent = Cast<UExponentialHeightFogComponent>(NewObject(GetClass()));
	if (DuplicatedComponent)
	{
		CopyCommonProperties(DuplicatedComponent);
		DuplicatedComponent->FogDensity = FogDensity;
		DuplicatedComponent->FogHeightFalloff = FogHeightFalloff;
		DuplicatedComponent->StartDistance = StartDistance;
		DuplicatedComponent->FogCutoffDistance = FogCutoffDistance;
		DuplicatedComponent->FogMaxOpacityDistance = FogMaxOpacityDistance;
		DuplicatedComponent->FogMaxOpacity = FogMaxOpacity;

		DuplicatedComponent->FogInscatteringColor = FogInscatteringColor;
	}
	DuplicatedComponent->DuplicateSubObjects();

	return nullptr;
}

void UExponentialHeightFogComponent::DuplicateSubObjects()
{
	USceneComponent::DuplicateSubObjects();
}
