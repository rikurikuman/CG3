#include "PolygonScene.h"
#include "TextDrawer.h"
#include "RInput.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "SimpleSceneTransition.h"

PolygonScene::PolygonScene()
{
	controlDescText0 = Sprite(TextDrawer::CreateStringTexture("ESC:終了", "", 24), { 0, 1 });
	controlDescText1 = Sprite(TextDrawer::CreateStringTexture("R: シーンチェンジ, T: 爆散", "", 24), { 0, 1 });
	controlDescText2 = Sprite(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), { 0, 1 });
	controlDescText3 = Sprite(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), { 0, 1 });
	controlDescText0.transform.position = { 0, (float)RWindow::GetHeight() - 72, 0 };
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 48, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText3.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText0.transform.UpdateMatrix();
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();
	controlDescText3.transform.UpdateMatrix();

	camera.viewProjection.eye = { 0, 0, -15 };
}

void PolygonScene::Init()
{
	Camera::nowCamera = &camera;
}

void PolygonScene::Update()
{
	if (RInput::GetInstance()->GetKeyDown(DIK_R)) {
		if (!SceneManager::IsSceneChanging()) {
			SceneManager::Change<GameScene, SimpleSceneTransition>();
		}
	}

	if (!exploded && RInput::GetInstance()->GetKeyDown(DIK_T)) {
		exploded = true;
		particleManager.SpawnPolygonExplostion(
			obj,
			{ -0.1f, -0.1f, -0.1f },
			{ 0.1f, 0.1f, 0.1f},
			{0, 0, 0},
			{0, 0, 0},
			10.0f
		);
	}

	camera.Update();

	obj.TransferBuffer(camera.viewProjection);

	particleManager.Update();
}

void PolygonScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

	if (!exploded) {
		obj.DrawCommands();
	}

	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

	controlDescText0.TransferBuffer();
	controlDescText1.TransferBuffer();
	controlDescText2.TransferBuffer();
	controlDescText3.TransferBuffer();
	controlDescText0.DrawCommands();
	controlDescText1.DrawCommands();
	controlDescText2.DrawCommands();
	controlDescText3.DrawCommands();

	particleManager.Draw();
}
