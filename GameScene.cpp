#include "GameScene.h"
#include "TextDrawer.h"
#include "RWindow.h"
#include "RInput.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"
#include "TimeManager.h"

GameScene::GameScene()
{
	skydome = ModelObj(Model::Load("Resources/Model/Skydome/", "skydome.obj"));

	controlDescText1 = Sprite(TextDrawer::CreateStringTexture("ESC:終了", "", 24), { 0, 1 });
	controlDescText2 = Sprite(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), { 0, 1 });
	controlDescText3 = Sprite(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), { 0, 1 });
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 48, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText3.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();
	controlDescText3.transform.UpdateMatrix();

	floor = Cube("");
	for (Image3D& face : floor.faces) {
		face.material.color = { 0.3f, 0.3f, 0.3f, 1.0f };
	}
	floor.transform.position = { 0.0f, -1.0f, 0.0f };
	floor.transform.scale = { 40.0f, 1.0f, 40.0f };
	floor.transform.UpdateMatrix();
	floor.UpdateFaces();

	ModelHandle objVicViperModel = Model::Load("./Resources/Model/VicViper/", "VicViper.obj");
	
	objVicViper = ModelObj(objVicViperModel);

	objVicViper.transform.position = { 0, 0, 0 };
	objVicViper.transform.scale = { 0.1f, 0.1f, 0.1f };
	objVicViper.transform.UpdateMatrix();

	camera.viewProjection.eye = { 0, 5, -20 };
}

void GameScene::Init()
{
	Camera::nowCamera = &camera;
}

void GameScene::Update()
{
	if (RInput::GetKeyDown(DIK_R)) {
		if(!SceneManager::IsSceneChanging()) SceneManager::Change<GameScene, SimpleSceneTransition>();
	}


	if (RInput::GetKey(DIK_LEFT)) {
		objVicViper.transform.rotation.y -= Util::AngleToRadian(2);
		objVicViper.transform.rotation.z += Util::AngleToRadian(1);
		if (Util::RadianToAngle(objVicViper.transform.rotation.z) >= 20) {
			objVicViper.transform.rotation.z = Util::AngleToRadian(20);
		}
	}
	else if(RInput::GetKey(DIK_RIGHT)) {
		objVicViper.transform.rotation.y += Util::AngleToRadian(2);
		objVicViper.transform.rotation.z -= Util::AngleToRadian(1);
		if (Util::RadianToAngle(objVicViper.transform.rotation.z) <= -20) {
			objVicViper.transform.rotation.z = Util::AngleToRadian(-20);
		}
	}
	else {
		if (objVicViper.transform.rotation.z != 0) {
			if (objVicViper.transform.rotation.z > 0) {
				objVicViper.transform.rotation.z -= Util::AngleToRadian(2);
				if (objVicViper.transform.rotation.z < 0) {
					objVicViper.transform.rotation.z = 0;
				}
			}
			else {
				objVicViper.transform.rotation.z += Util::AngleToRadian(2);
				if (objVicViper.transform.rotation.z > 0) {
					objVicViper.transform.rotation.z = 0;
				}
			}
		}
	}

	if (RInput::GetKey(DIK_G)) {
		objVicViper.transform.position.y = -0.2f;

		speed -= 0.05f * speed;
		if (speed <= 0) {
			speed = 0;
		}
		charge += TimeManager::deltaTime;
	}
	else {
		objVicViper.transform.position.y = 0.0f;

		if (charge >= 0.5f) {
			speed = 0.4f;
		}
		charge = 0;
		
		speed += 0.2f * TimeManager::deltaTime;
	}

	if (speed > 0) {
		speed -= 0.02f * speed;
		if (speed <= 0) {
			speed = 0;
		}
	}
	

	Vector3 moveVec = { 0, 0, 1 };
	moveVec *= Matrix4::RotationY(objVicViper.transform.rotation.y);

	objVicViper.transform.position += moveVec * speed;
	objVicViper.transform.position.x = Util::Clamp(objVicViper.transform.position.x, -20.0f, 20.0f);
	objVicViper.transform.position.z = Util::Clamp(objVicViper.transform.position.z, -20.0f, 20.0f);

	objVicViper.transform.UpdateMatrix();

	camera.Update();

	floor.TransferBuffer(camera.viewProjection);

	objVicViper.TransferBuffer(camera.viewProjection);

	skydome.transform.scale = { 4,4,4 };
	skydome.transform.UpdateMatrix();
	skydome.TransferBuffer(camera.viewProjection);
}

void GameScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

	floor.DrawCommands();
	objVicViper.DrawCommands();
	skydome.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetPipelineState(TextDrawer::GetInstance()->pipeline.ptr.Get());

	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

	controlDescText1.TransferBuffer();
	controlDescText2.TransferBuffer();
	controlDescText3.TransferBuffer();
	controlDescText1.DrawCommands();
	controlDescText2.DrawCommands();
	controlDescText3.DrawCommands();
}
