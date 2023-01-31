#include "ResultScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "SimpleSceneTransition.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "GameData.h"
#include "RAudio.h"

ResultScene::ResultScene()
{
	light.lightVec = { 1, -1, 1 };
	light.TransferBuffer();

	camera.viewProjection.eye = { 0, 3, -5 };
	camera.viewProjection.UpdateMatrix();

	objC = ModelObj(Model::LoadWithAIL("./Resources/Model/pCourse", "pCourse_Terrain.obj"));

	screen = Sprite(TextureManager::Load("./Resources/resultScreen.png", "resultScreen"), { 0.5f, 0 });
	screen.transform.position = { RWindow::GetWidth() / 2.0f, 0, 0 };
	screen.transform.UpdateMatrix();
	screen.TransferBuffer();
}

void ResultScene::Init()
{
	Camera::nowCamera = &camera;
	Light::nowLight = &light;
	RAudio::Stop("BGM_Game");
	RAudio::Play("BGM_Result", 0.5f, 1.0f, true);
}

void ResultScene::Update()
{
	if (RInput::GetKeyDown(DIK_SPACE)) {
		if (!SceneManager::IsSceneChanging()) SceneManager::Change<TitleScene, SimpleSceneTransition>();
	}

	objC.transform.position = { 1.5f, 0, 0 };
	objC.transform.rotation.y += Util::AngleToRadian(2);
	objC.transform.scale = { 0.08f, 0.08f, 0.08f };
	objC.transform.UpdateMatrix();

	objC.TransferBuffer(Camera::nowCamera->viewProjection);
}

void ResultScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

	screen.DrawCommands();

	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());

	objC.DrawCommands();

	SimpleDrawer::DrawString(490, 395, Util::StringFormat("%4.2fs", GameData::goalTime), { 1, 0.9f, 0.2f, 1 }, "", 100, { 1, 1 });
}
