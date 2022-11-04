#include "SimpleSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

SimpleSceneTransition::SimpleSceneTransition() {
	shutter = Sprite(TextureManager::Load("Resources/shutter.png"), {0.0f, 1.0f});
}

void SimpleSceneTransition::Update()
{
	if (closeFlag) {
		if (shutterPos >= 1) {
			inProgress = false;
			shutterPos = 1;
		}
		else {
			inProgress = true;
			if (timer == 0) {
				//PlaySE("ShutterClose");
			}
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.8f);

			shutterPos = (1.0f - powf(1 - t, 5));
		}
	}
	else {
		if (shutterPos <= 0) {
			inProgress = false;
			shutterPos = 0;
		}
		else {
			inProgress = true;
			if (timer == 0) {
				//PlaySE("ShutterOpen");
			}
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.8f);

			shutterPos = 1 - (1.0f - powf(1 - t, 5));
		}
	}

	shutter.transform.position = { 0, RWindow::GetHeight() * shutterPos, 0 };
	shutter.transform.UpdateMatrix();
	shutter.TransferBuffer();
}

void SimpleSceneTransition::Draw()
{
	if (shutterPos != 0) {
		shutter.DrawCommands();
		//SimpleDrawer::DrawBox(0, 0, (float)RWindow::GetWidth(), (float)RWindow::GetHeight() * shutterPos, Color(0, 0.5f, 0.5f, 1), true);
	}
}

void SimpleSceneTransition::Open()
{
	closeFlag = false;
	timer = 0;
}

bool SimpleSceneTransition::IsOpened()
{
	return !closeFlag && !inProgress && timer != 0;
}

void SimpleSceneTransition::Close()
{
	closeFlag = true;
	timer = 0;
}

bool SimpleSceneTransition::IsClosed()
{
	return closeFlag && !inProgress && timer != 0;
}

bool SimpleSceneTransition::InProgress()
{
	return inProgress;
}