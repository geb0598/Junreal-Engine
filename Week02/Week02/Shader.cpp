#include "pch.h"
#include "Shader.h"

UShader::~UShader()
{
    ReleaseResources();
}

// 두 개의 셰이더 파일을 받는 주요 Load 함수
void UShader::Load(const FString& InShaderPath, ID3D11Device* InDevice, EVertexLayoutType InLayoutType)
{
    assert(InDevice);

    std::wstring WFilePath;
    WFilePath = std::wstring(InShaderPath.begin(), InShaderPath.end());

    D3DCompileFromFile(WFilePath.c_str(), nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &VSBlob, nullptr);

    InDevice->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &VertexShader);

    D3DCompileFromFile(WFilePath.c_str(), nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &PSBlob, nullptr);

    InDevice->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &PixelShader);

    CreateInputLayout(InDevice, InLayoutType);
}

void UShader::CreateInputLayout(ID3D11Device* Device, EVertexLayoutType LayoutType)
{
    const D3D11_INPUT_ELEMENT_DESC* layout = nullptr;
    uint32 layoutCount = 0;

    switch (LayoutType)
    {
    case EVertexLayoutType::PositionColor:
        layout = FVertexPositionColor::GetLayout();
        layoutCount = FVertexPositionColor::GetLayoutCount();
        break;

    case EVertexLayoutType::PositionColorTexturNormal:
        layout = FVertexPositionColorTexturNormal::GetLayout();
        layoutCount = FVertexPositionColorTexturNormal::GetLayoutCount();
        break;

    case EVertexLayoutType::PositionBillBoard:
        layout = FVertexPositionBillBoard::GetLayout();
        layoutCount = FVertexPositionBillBoard::GetLayoutCount();
        break;
    }

    HRESULT hr = Device->CreateInputLayout(
        layout,
        layoutCount,
        VSBlob->GetBufferPointer(), 
        VSBlob->GetBufferSize(),
        &InputLayout);
    assert(SUCCEEDED(hr));
}

void UShader::ReleaseResources()
{
    if (VSBlob)
    {
        VSBlob->Release();
        VSBlob = nullptr;
    }
    if (PSBlob)
    {
        PSBlob->Release();
        PSBlob = nullptr;
    }
    if (InputLayout)
    {
        InputLayout->Release();
        InputLayout = nullptr;
    }
    if (VertexShader)
    {
        VertexShader->Release();
        VertexShader = nullptr;
    }
    if (PixelShader)
    {
        PixelShader->Release();
        PixelShader = nullptr;
    }
}
