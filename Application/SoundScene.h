#pragma once
#include "IScene.h"
#include "RAudio.h"

class SoundScene : public IScene
{
public:
	SoundScene();

	void Init() override;
	void Update() override;

private:
	AudioHandle handle;

	bool autoPlay = false;
	float volume = 1.0f;
	float pitch = 1.0f;
};

