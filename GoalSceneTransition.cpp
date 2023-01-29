#include "GoalSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

GoalSceneTransition::GoalSceneTransition() {
	shutter = Sprite("", { 0.5f, 0.5f });
}

void GoalSceneTransition::Update()
{
	if (closeFlag) {
		if (shutterAlpha >= 1 && timer >= 2.2f) {
			inProgress = false;
			shutterAlpha = 1;
		}
		else {
			inProgress = true;
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 1.0f);

			shutterAlpha = 1.0f * t;
		}
	}
	else {
		if (shutterAlpha <= 0) {
			inProgress = false;
			shutterAlpha = 0;
		}
		else {
			inProgress = true;
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.7f);

			shutterAlpha = 1 - (1.0f - powf(1 - t, 5));
		}
	}

	shutter.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 0 };
	shutter.transform.rotation = { 0, 0, 0 };
	shutter.transform.scale = { 100000, 100000, 1 };
	shutter.material.color = { 1, 1, 1, shutterAlpha };
	shutter.transform.UpdateMatrix();
	shutter.TransferBuffer();
}

void GoalSceneTransition::Draw()
{
	if (shutterAlpha != 0) {
		shutter.DrawCommands();
		//SimpleDrawer::DrawBox(0, 0, (float)RWindow::GetWidth(), (float)RWindow::GetHeight() * shutterPos, Color(0, 0.5f, 0.5f, 1), true);
	}
}

void GoalSceneTransition::Open()
{
	closeFlag = false;
	timer = 0;
}

bool GoalSceneTransition::IsOpened()
{
	return !closeFlag && !inProgress && timer != 0;
}

void GoalSceneTransition::Close()
{
	closeFlag = true;
	timer = 0;
}

bool GoalSceneTransition::IsClosed()
{
	return closeFlag && !inProgress && timer != 0;
}

bool GoalSceneTransition::InProgress()
{
	return inProgress;
}