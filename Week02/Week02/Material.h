#pragma once
#include <d3d11.h>
#include "Object.h"
class UMaterial : public UObject
{
public:
    UMaterial() {};

protected:
    ~UMaterial() override {};

public:
	ID3D11VertexShader* GetVertexShader() const { return SimpleVertexShader; }
	ID3D11PixelShader* GetPixelShader() const { return SimplePixelShader; }
	ID3D11InputLayout* GetInputLayout() const { return SimpleInputLayout; }
	ID3D11Buffer* GetConstantBuffer() const { return ConstantBuffer; }
	UINT GetCBufferSize() const { return CBufferSize; }
	UINT GetCBufferSlot() const { return CBufferSlot; }

	void SetVertexShader(ID3D11VertexShader* shader) { SimpleVertexShader = shader; }
	void SetPixelShader(ID3D11PixelShader* shader) { SimplePixelShader = shader; }
	void SetInputLayout(ID3D11InputLayout* layout) { SimpleInputLayout = layout; }
	void SetConstantBuffer(ID3D11Buffer* buffer, UINT size, UINT slot) { ConstantBuffer = buffer; CBufferSize = size; CBufferSlot = slot; }

private:

	ID3D11VertexShader* SimpleVertexShader = nullptr;
	ID3D11PixelShader* SimplePixelShader = nullptr;
	ID3D11InputLayout* SimpleInputLayout = nullptr;

	ID3D11Buffer* ConstantBuffer;
	UINT CBufferSize = 0;
	UINT CBufferSlot = 0;
};

