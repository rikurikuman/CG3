#pragma once
#include "IScene.h"
class VerySlowLoadScene : public IScene
{
public:
	VerySlowLoadScene();

	void Init() override;
	void Draw() override;
};

