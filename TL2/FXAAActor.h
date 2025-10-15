#pragma once
#include "Info.h"
class UFXAAComponent;

class AFXAAActor : public AInfo
{
public:

	DECLARE_CLASS(AFXAAActor, AInfo);
	AFXAAActor();

private:
	UFXAAComponent* FXAAComponent = nullptr;
	UBillboardComponent* SpriteComponent = nullptr;

};

