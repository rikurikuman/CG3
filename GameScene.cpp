#include "GameScene.h"
#include "TextDrawer.h"
#include "RWindow.h"
#include "RInput.h"
#include "SceneManager.h"
#include "PolygonScene.h"
#include "SimpleSceneTransition.h"

GameScene::GameScene()
{
	skydome = ModelObj(Model::Load("Resources/skydome/", "skydome.obj"));

	controlDescText0 = Sprite(TextDrawer::CreateStringTexture("ESC:終了", "", 24), { 0, 1 });
	controlDescText1 = Sprite(TextDrawer::CreateStringTexture("R: シーンチェンジ", "", 24), { 0, 1 });
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

	TextureHandle tex = TextureManager::Load("Resources/conflict.jpg");
	cube = Cube(tex);
	cube.transform.position = { 0, 0, 10 };

	text = Image3D(TextDrawer::CreateStringTexture("hogehogeあいうえintほげ", "ＭＳ Ｐ明朝", 128), { 3.0f, 3.0f });
	text.transform.position = { 0, 0, 20 };
	text.transform.UpdateMatrix();

	camera.viewProjection.eye = { 0, 0, -10 };

	//グリッド
	gridRoot = RDirectX::GetInstance()->rootSignature;

	RootParamaters gridRootParams(2);
	//定数バッファ0番(Material)
	gridRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	gridRootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	gridRootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	gridRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番(ViewProjection)
	gridRootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	gridRootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	gridRootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	gridRootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	gridRoot.desc.RootParamaters = gridRootParams;
	gridRoot.Create();

	gridPSO = RDirectX::GetInstance()->pipelineState;
	gridPSO.desc.VS = Shader("Shader/GridVS.hlsl", "main", "vs_5_0");
	gridPSO.desc.PS = Shader("Shader/GridPS.hlsl", "main", "ps_5_0");

	InputLayout gridInputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};
	gridPSO.desc.InputLayout = gridInputLayout;
	gridPSO.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	gridPSO.desc.pRootSignature = gridRoot.ptr.Get();
	gridPSO.Create();

	for (int i = -100; i <= 100; i++) {
		gridVertices.push_back(Vector3{ (float)i, -10, 100 });
		gridVertices.push_back(Vector3{ (float)i, -10, -100 });
	}

	for (int i = -100; i <= 100; i++) {
		gridVertices.push_back(Vector3{ 100, -10, (float)i });
		gridVertices.push_back(Vector3{ -100, -10, (float)i });
	}

	gridVertBuff = VertexBuffer(gridVertices);

	gridMaterial.color = Color(0, 1, 1, 0.25f);
	gridMaterial.Transfer(gridMaterialBuff.constMap);

	patEmitterA = new ParticleEmitter(&particleManager);
	patEmitterB = new ParticleEmitter(&particleManager);
	patEmitterC = new ParticleEmitter(&particleManager);

	patEmitterA->spawnCount = 10;
	patEmitterA->spawnInterval = 4.5f;
	patEmitterA->timer = 4.5f;
	patEmitterA->minAliveTime = 5.0f;
	patEmitterA->maxAliveTime = 5.0f;

	patEmitterB->pos = { -5, 0, 0 };
	patEmitterB->spawnAreaSize = { 0, 0, 0 };
	patEmitterB->minVelocity = { -0.1f, -0.1f, -0.1f };
	patEmitterB->maxVelocity = { 0.1f, 0.1f, 0.1f };
	patEmitterB->color = { 1, 0.1f, 0.1f, 1 };
	patEmitterB->endColor = { 0.1f, 0.1f, 1, 1 };
	patEmitterB->spawnInterval = 0.1f;
	patEmitterB->spawnCount = 20;

	patEmitterC->pos = { 5, 0, 0 };
	patEmitterC->spawnAreaSize = { 0.2f, 0.2f, 0.2f };
	patEmitterC->minVelocity = { -0.01f, 0.01f, -0.01f };
	patEmitterC->maxVelocity = { 0.01f, 0.05f, 0.01f };
	patEmitterC->color = { 1, 0.3f, 0.1f, 1 };
	patEmitterC->endColor = { 1, 0.3f, 0.1f, 1 };
	patEmitterC->spawnInterval = 0.01f;
	patEmitterC->spawnCount = 5;
}

GameScene::~GameScene() {
	delete patEmitterA;
	delete patEmitterB;
	delete patEmitterC;
}

void GameScene::Init()
{
	Camera::nowCamera = &camera;
}

void GameScene::Update()
{
	if (RInput::GetInstance()->GetKeyDown(DIK_R)) {
		if (!SceneManager::IsSceneChanging()) {
			SceneManager::Change<PolygonScene, SimpleSceneTransition>();
		}
	}

	cube.transform.rotation.y += Util::AngleToRadian(1);
	cube.transform.UpdateMatrix();
	cube.UpdateFaces();

	camera.Update();

	text.TransferBuffer(camera.viewProjection);

	cube.TransferBuffer(camera.viewProjection);

	skydome.transform.scale = { 4,4,4 };
	skydome.transform.UpdateMatrix();
	skydome.TransferBuffer(camera.viewProjection);

	gridViewProjectionBuff.constMap->matrix = camera.viewProjection.matrix;

	patEmitterA->Update();
	patEmitterB->Update();
	patEmitterC->Update();
	particleManager.Update();
}

void GameScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

	cube.DrawCommands();
	skydome.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetPipelineState(TextDrawer::GetInstance()->pipeline.ptr.Get());
	text.DrawCommands();

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

	//グリッド
	RDirectX::GetInstance()->cmdList->SetPipelineState(gridPSO.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(gridRoot.ptr.Get());
	RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &gridVertBuff.view);
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(0, gridMaterialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, gridViewProjectionBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->DrawInstanced((UINT)gridVertices.size(), 1, 0, 0);
	RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
