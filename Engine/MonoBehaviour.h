#pragma once

#include "Component.h"

class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

private:
	virtual void FinalUpdate() sealed {}	// 상속을 활용할 수 없도록 봉인(sealed)
};

