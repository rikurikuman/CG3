#include "GameScene.h"
#include "TextDrawer.h"
#include "RWindow.h"
#include "RInput.h"
#include "SceneManager.h"
#include "GoalSceneTransition.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "GameData.h"
#include "ResultScene.h"
#include "RAudio.h"

GameScene::GameScene()
{
	controlDescText1 = Sprite(TextDrawer::CreateStringTexture("ESC:終了", "", 24), { 0, 1 });
	controlDescText2 = Sprite(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), { 0, 1 });
	controlDescText3 = Sprite(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), { 0, 1 });
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 48, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText3.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();
	controlDescText3.transform.UpdateMatrix();

	countText1 = Sprite(TextDrawer::CreateStringTexture("3", "", 400), { 0.5f, 0.5f });
	countText2 = Sprite(TextDrawer::CreateStringTexture("2", "", 400), { 0.5f, 0.5f });
	countText3 = Sprite(TextDrawer::CreateStringTexture("1", "", 400), { 0.5f, 0.5f });
	countText4 = Sprite(TextDrawer::CreateStringTexture("START", "", 300), { 0.5f, 0.5f });
	countText1.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 0 };
	countText2.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 0 };
	countText3.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 0 };
	countText4.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 0 };
	countText1.transform.UpdateMatrix();
	countText2.transform.UpdateMatrix();
	countText3.transform.UpdateMatrix();
	countText4.transform.UpdateMatrix();
	countText1.TransferBuffer();
	countText2.TransferBuffer();
	countText3.TransferBuffer();
	countText4.TransferBuffer();

	goalText = Sprite(TextDrawer::CreateStringTexture("GOAL!!", "", 300), { 0.5f, 0.5f });
	goalText.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 0 };
	goalText.transform.UpdateMatrix();
	goalText.TransferBuffer();

	descImg = Sprite(TextureManager::Load("./Resources/desc.png", "descImg"));
	descImg.transform.position = { 225.0f, RWindow::GetHeight() - 45.0f, 0};
	descImg.transform.UpdateMatrix();
	descImg.TransferBuffer();

	monitorImg = Sprite(TextureManager::Load("./Resources/monitor.png", "monitorImg"), { 1.0f, 0.0f });
	monitorImg.transform.position = { (float)RWindow::GetWidth() - 20.0f, 0, 0 };
	monitorImg.transform.UpdateMatrix();
	monitorImg.TransferBuffer();

	meterImg = Sprite(TextureManager::Load("./Resources/meter.png", "meterImg"), {1.0f, 1.0f});
	meterImg.transform.position = { (float)RWindow::GetWidth(), (float)RWindow::GetHeight(), 0 };
	meterImg.transform.UpdateMatrix();
	meterImg.TransferBuffer();

	player.course = &course;

	camera.viewProjection.eye = { 0, 5, -20 };
}

void GameScene::Init()
{
	Camera::nowCamera = &player.camera;
	Light::nowLight = &light;
	RAudio::Stop("BGM_Title");
}

void GameScene::Update()
{
	if (!startFlag) {
		startTimer += TimeManager::deltaTime;

		if (startCount == 0) {
			if (startTimer >= 1.0f) {
				startTimer = 0;
				startCount++;
				RAudio::Play("BGM_Game", 0.5f, 1.0f, true);
			}
		}
		else if (startCount == 1) {
			if (startTimer >= 2.0f) {
				startTimer = 0;
				startCount++;
				RAudio::Play("Countdown");
			}
		}
		else {
			if (startCount <= 4) {
				if (startTimer >= 1.0f) {
					startTimer = 0;
					startCount++;
					if (startCount == 5) {
						RAudio::Play("RunStart");
					}
					else {
						RAudio::Play("Countdown");
					}
				}
			}
			else {
				startFlag = true;
			}
		}
	}
	else {
		if (startTimer <= 5.0f) {
			startTimer += TimeManager::deltaTime;
		}
		player.startFlag = true;

		if (!goalFlag) {
			runTimer += TimeManager::deltaTime;

			if (player.lap >= 5) {
				goalFlag = true;
				RAudio::Play("Finish");
				GameData::goalTime = runTimer;
			}
		}

		if (goalFlag) {
			goalTimer += TimeManager::deltaTime;
			if (goalTimer >= 5.0f) {
				if (!SceneManager::IsSceneChanging()) SceneManager::Change<ResultScene, GoalSceneTransition>();
			}
		}
	}
	
	if (RInput::GetKey(DIK_J)) {
		light.lightVec.x -= 0.1f;
	}
	if (RInput::GetKey(DIK_L)) {
		light.lightVec.x += 0.1f;
	}
	light.TransferBuffer();

	player.Update();
	course.Update();

	camera.Update();
}

void GameScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

	player.Draw();
	course.Draw();

	RDirectX::GetInstance()->cmdList->SetPipelineState(TextDrawer::GetInstance()->pipeline.ptr.Get());

	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

	controlDescText1.TransferBuffer();
	controlDescText2.TransferBuffer();
	controlDescText3.TransferBuffer();
	controlDescText1.DrawCommands();
	controlDescText2.DrawCommands();
	controlDescText3.DrawCommands();

	descImg.DrawCommands();
	monitorImg.DrawCommands();
	meterImg.DrawCommands();

	if (!startFlag) {
		if (startCount == 2) {
			countText1.material.color = { 1, 0, 0, 1 };
			countText1.TransferBuffer();
			countText1.DrawCommands();
		}
		else if (startCount == 3) {
			countText2.material.color = { 1, 0, 0, 1 };
			countText2.TransferBuffer();
			countText2.DrawCommands();
		}
		else if (startCount == 4) {
			countText3.material.color = { 1, 0, 0, 1 };
			countText3.TransferBuffer();
			countText3.DrawCommands();
		}
	}
	else {
		if (startTimer <= 6) {
			float alp = max(0, startTimer - 2.0f / 3.0f);
			countText4.material.color = { 1, 0, 0, 1 - alp };
			countText4.TransferBuffer();
			countText4.DrawCommands();
		}
	}

	if (goalFlag) {
		goalText.material.color = { 1, 0, 0, 1 };
		goalText.TransferBuffer();
		goalText.DrawCommands();
	}

	SimpleDrawer::DrawString(1262, 135, Util::StringFormat("%4.2fs", runTimer), {1, 0.9f, 0.2f, 1}, "", 40, {1, 0});

	SimpleDrawer::DrawString(975, 635, Util::StringFormat("%4.1f", player.speed * 100), {1, 1, 1, 1}, "", 65, {0, 0});
	if (player.charge / 0.5f >= 1.0f) {
		SimpleDrawer::DrawBox(1205, 534, 1276, 717, Color::convertFromHSVA({ 0, 100, 100, 255 }), true);
	}
	else {
		SimpleDrawer::DrawBox(1205, 717 - 182 * min(1, player.charge / 0.5f), 1276, 717, Color::convertFromHSVA({ (int)(200 - 190 * min(1, player.charge / 0.5f)), 100, 100, 255 }), true);
	}
	
	SimpleDrawer::DrawString(1090, 30, Util::StringFormat("%2d", min(player.lap + 1, 5)), { 1, 1, 1, 1 }, "", 80);
	SimpleDrawer::DrawString(1210, 80, "/ 5", {1, 1, 1, 1}, "", 30);
}
