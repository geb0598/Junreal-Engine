#pragma once
#include "Info.h"
class UExponentialHeightFogComponent;

class AExponentialHeightFog : public AInfo
{
public:

	DECLARE_CLASS(AExponentialHeightFog, AInfo);


	AExponentialHeightFog();
	UObject* Duplicate() override;
	void DuplicateSubObjects() override;

private:
	UExponentialHeightFogComponent* HeightFogComponent = nullptr;
	UBillboardComponent* SpriteComponent = nullptr;

};

