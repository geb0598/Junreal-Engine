#pragma once

class URenderer;
class UShader;

class UFXAA
{
public:
	UFXAA();
	void Render(URenderer* Renderer);
private:
	UShader* Shader;
};