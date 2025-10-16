#pragma once
#include "Info.h"
class UFXAAComponent;

class AFXAAActor : public AInfo
{
public:

	DECLARE_CLASS(AFXAAActor, AInfo);
	AFXAAActor();
	UObject* Duplicate() override;
	void DuplicateSubObjects() override;

private:
	UFXAAComponent* FXAAComponent = nullptr;
	UBillboardComponent* SpriteComponent = nullptr;

};

