#include "TitleScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "SimpleSceneTransition.h"
#include "TimeManager.h"
#include "RAudio.h"

TitleScene::TitleScene()
{
	light.lightVec = { 1, -1, 1 };
	light.TransferBuffer();

	camera.viewProjection.eye = { 0, 3, -5 };
	camera.viewProjection.UpdateMatrix();

	objVic = ModelObj(Model::Load("./Resources/Model/VicViper", "VicViper.obj"));

	logo = Sprite(TextureManager::Load("./Resources/titleLogo.png", "titleLogo"), {0.5f, 0});
	logo.transform.position = { RWindow::GetWidth() / 2.0f, 0, 0 };
	logo.transform.UpdateMatrix();
	logo.TransferBuffer();

	startBar = Sprite(TextureManager::Load("./Resources/startBar.png", "startBar"), { 0.5f, 0 });
	startBar.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() - 200.0f, 0};
	startBar.transform.UpdateMatrix();
	startBar.TransferBuffer();
}

void TitleScene::Init()
{
	Camera::nowCamera = &camera;
	Light::nowLight = &light;
	RAudio::Stop("BGM_Result");
	RAudio::Play("BGM_Title", 0.5f, 1.0f, true);
}

void TitleScene::Update()
{
	if (RInput::GetKeyDown(DIK_SPACE)) {
		if (!SceneManager::IsSceneChanging()) {
			SceneManager::Change<GameScene, SimpleSceneTransition>();
			RAudio::Play("Start");
		}
	}

	objVic.transform.rotation.y += Util::AngleToRadian(2);
	objVic.transform.scale = { 0.2f, 0.2f, 0.2f };
	objVic.transform.UpdateMatrix();

	objVic.TransferBuffer(Camera::nowCamera->viewProjection);

	startTimer += TimeManager::deltaTime;
	startBar.material.color = { 1, 1, 1, 0.7f + 0.3f * sinf(startTimer * 10.0f) };
	startBar.TransferBuffer();
}

void TitleScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

	objVic.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
	
	logo.DrawCommands();
	startBar.DrawCommands();
}


