#pragma once
#include "SceneTransition.h"
#include "Sprite.h"

class GoalSceneTransition : public SceneTransition
{
private:
	Sprite shutter;

	bool inProgress = false;
	float timer = 0;
	float shutterAlpha = 0;
	float shutterElapsedTime = 0;

public:
	GoalSceneTransition();

	virtual void Update() override;
	virtual void Draw() override;

	//トランジションを開ける
	virtual void Open() override;
	//トランジションが開ききっているか
	virtual bool IsOpened() override;
	//トランジションを閉める
	virtual void Close() override;
	//トランジションが閉まりきっているか
	virtual bool IsClosed() override;
	//トランジションが動作中か
	virtual bool InProgress() override;
};