#include "GameScene.h"
#include "TextDrawer.h"
#include "RWindow.h"
#include "RInput.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"

GameScene::GameScene()
{
	skydome = ModelObj(Model::Load("Resources/skydome/", "skydome.obj"));

	controlDescText1 = Sprite(TextDrawer::CreateStringTexture("ESC:終了", "", 24), { 0, 1 });
	controlDescText2 = Sprite(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), { 0, 1 });
	controlDescText3 = Sprite(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), { 0, 1 });
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 48, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText3.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();
	controlDescText3.transform.UpdateMatrix();

	TextureHandle tex = TextureManager::Load("Resources/conflict.jpg");
	cube = Cube(tex);
	cube.transform.position = { 0, 0, 10 };

	model = ModelObj(Model::LoadWithAIL("Resources/Model/VicViper", "VicViper.fbx"));
	//model = ModelObj(Model::Load("./Resources/Model/VicViper/", "VicViper.obj"));
	model.transform.scale = { 0.2f, 0.2f, 0.2f };
	model.transform.UpdateMatrix();

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

	cube.transform.rotation.y += Util::AngleToRadian(1);
	cube.transform.UpdateMatrix();
	cube.UpdateFaces();

	camera.Update();

	text.TransferBuffer(camera.viewProjection);

	cube.TransferBuffer(camera.viewProjection);
	model.TransferBuffer(camera.viewProjection);

	skydome.transform.scale = { 4,4,4 };
	skydome.transform.UpdateMatrix();
	skydome.TransferBuffer(camera.viewProjection);

	gridViewProjectionBuff.constMap->matrix = camera.viewProjection.matrix;
}

void GameScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

	cube.DrawCommands();
	model.DrawCommands();
	skydome.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetPipelineState(TextDrawer::GetInstance()->pipeline.ptr.Get());
	text.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

	controlDescText1.TransferBuffer();
	controlDescText2.TransferBuffer();
	controlDescText3.TransferBuffer();
	controlDescText1.DrawCommands();
	controlDescText2.DrawCommands();
	controlDescText3.DrawCommands();

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
