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

	//�g�����W�V�������J����
	virtual void Open() override;
	//�g�����W�V�������J�������Ă��邩
	virtual bool IsOpened() override;
	//�g�����W�V������߂�
	virtual void Close() override;
	//�g�����W�V�������܂肫���Ă��邩
	virtual bool IsClosed() override;
	//�g�����W�V���������쒆��
	virtual bool InProgress() override;
};