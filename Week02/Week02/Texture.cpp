#include "pch.h"
#include "Texture.h"
#include <DDSTextureLoader.h>

UTexture::UTexture()
{
	Width = 0;
	Height = 0;
	Format = DXGI_FORMAT_UNKNOWN;
}

UTexture::~UTexture()
{
	ReleaseResources();
}

void UTexture::Load(const FString& InFilePath, ID3D11Device* InDevice)
{
	assert(InDevice);

	std::wstring WFilePath;
	WFilePath = std::wstring(InFilePath.begin(), InFilePath.end());

	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		InDevice,
		WFilePath.c_str(),
		reinterpret_cast<ID3D11Resource**>(&Texture2D),
		&ShaderResourceView
	);
	if (FAILED(hr))
	{
		UE_LOG("!!!LOAD TEXTIRE FAILED!!!");
	}

	if (Texture2D)
	{
		D3D11_TEXTURE2D_DESC desc;
		Texture2D->GetDesc(&desc);
		Width = desc.Width;
		Height = desc.Height;
		Format = desc.Format;
	}

	CreateSamplerState(InDevice);
	//UE_LOG("test output : %s",FString("helloworld"));
	UE_LOG("Successfully loaded DDS texture: %s", InFilePath);
}

void UTexture::CreateSamplerState(ID3D11Device* Device)
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = Device->CreateSamplerState(&samplerDesc, &SamplerState);
	assert(SUCCEEDED(hr));
}

void UTexture::ReleaseResources()
{
	if(Texture2D)
	{
		Texture2D->Release();
	}

	if(ShaderResourceView)
	{
		ShaderResourceView->Release();
	}

	if(SamplerState)
	{
		SamplerState->Release();
	}

	Width = 0;
	Height = 0;
	Format = DXGI_FORMAT_UNKNOWN;
}
