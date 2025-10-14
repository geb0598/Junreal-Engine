#include "pch.h"
#include "ExponentialHeightFogComponent.h"
#include "MeshLoader.h"

void UExponentialHeightFogComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Projection, FViewport* Viewport)
{

	//상수버퍼 업데이트
	Renderer->UpdateCBuffer(FHeightFogBufferType(
		FogInscatteringColor,
		FogDensity,
		FogHeightFalloff,
		StartDistance,
		FogCutoffDistance,
		FogMaxOpacity,
		this->GetWorldLocation().Z));

	FMatrix ViewProj = View * Projection;
	Renderer->UpdateCBuffer(ViewProjBufferType(View, Projection));
	Renderer->UpdateCBuffer(FViewProjectionInverse(ViewProj.Inverse()));
	
	Renderer->UpdateCBuffer(ViewportBufferType(FVector4(
	Viewport->GetStartX(),
	Viewport->GetStartY(),
	Viewport->GetSizeX(),
	Viewport->GetSizeY())));

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
