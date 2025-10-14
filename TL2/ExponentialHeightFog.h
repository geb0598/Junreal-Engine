#pragma once
#include "Info.h"
class UExponentialHeightFogComponent;

class AExponentialHeightFog : public AInfo
{
public:

	DECLARE_CLASS(AExponentialHeightFog, AInfo);


	AExponentialHeightFog();

private:
	UExponentialHeightFogComponent* HeightFogComponent = nullptr;
	UBillboardComponent* SpriteComponent = nullptr;

};

