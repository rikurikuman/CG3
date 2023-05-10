#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>

MainTestScene::MainTestScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.transform.position = { 0, 0, 0 };
	sphere.transform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));

	sphere2.transform.position = { 0.5f, 0, -1 };
	sphere2.transform.UpdateMatrix();

	sphere2.tuneMaterial.color.a = 0.6f;

	sprite.SetTexture("");
	sprite.SetAnchor({ 0, 0 });
	sprite.transform.position = { 100, 100, 0 };
	sprite.transform.UpdateMatrix();
	sprite.material.color = { 1, 1, 1, 1 };

	sprite2.SetTexture("");
	sprite2.SetAnchor({ 0, 0 });
	sprite2.transform.position = { 120, 120, 0 };
	sprite2.transform.UpdateMatrix();
	sprite2.material.color = { 1, 0, 0, 0.5f };

	camera.viewProjection.eye = { 0, 0, -10 };
	camera.viewProjection.target = { 0, 0, 0 };
	camera.viewProjection.UpdateMatrix();
}

void MainTestScene::Init()
{
	testRootSignature = RDirectX::GetDefRootSignature();

	// ルートパラメータの設定
	RootParamaters rootParams(1);
	//定数バッファ0番(VP)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	testRootSignature.desc.RootParamaters = rootParams;
	testRootSignature.Create();

	testPipeline = RDirectX::GetDefPipeline();
	testPipeline.desc.VS = Shader("Shader/TestVS.hlsl", "main", "vs_5_0");
	testPipeline.desc.PS = Shader("Shader/TestPS.hlsl", "main", "ps_5_0");
	testPipeline.desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		}
	};
	testPipeline.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	testPipeline.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	testPipeline.desc.RasterizerState.DepthClipEnable = false;
	testPipeline.desc.DepthStencilState.DepthEnable = false;
	testPipeline.desc.pRootSignature = testRootSignature.ptr.Get();
	testPipeline.Create();

	VertexPU verts[3] = {
		{{0, -10, 0},  {0.5f, 0}},
		{{-10, 10, 0}, {0, 1}},
		{{10, 10, 0}, {1, 1}}
	};

	vertBuffT = SRBufferAllocator::Alloc(sizeof(VertexPU) * 3, 1);

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexPU* vertMap = reinterpret_cast<VertexPU*>(vertBuffT.Get());
	//全頂点に対して
	for (UINT i = 0; i < 3; i++) {
		vertMap[i] = verts[i];
	}

	//頂点バッファビューの作成
	vertBuffView.BufferLocation = vertBuffT.GetGPUVirtualAddress(); //GPU仮想アドレス
	vertBuffView.SizeInBytes = sizeof(VertexPU) * 3; //頂点バッファのサイズ
	vertBuffView.StrideInBytes = sizeof(VertexPU); //頂点一個のサイズ

	std::vector<Vector3> positions(5000);
	for (int i = 0; i < 5000; i++) {
		positions[i] = { Util::GetRand(0.0f, RWindow::GetWidth() / 1.0f), Util::GetRand(0.0f, RWindow::GetHeight() / 1.0f), 0.0f };
	}

	instanceVertBuffT = SRBufferAllocator::Alloc(sizeof(Vector3) * 5000, 1);

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	Vector3* instanceVertMap = reinterpret_cast<Vector3*>(instanceVertBuffT.Get());
	//全頂点に対して
	for (UINT i = 0; i < 5000; i++) {
		instanceVertMap[i] = positions[i];
	}

	//頂点バッファビューの作成
	instanceVertBuffView.BufferLocation = instanceVertBuffT.GetGPUVirtualAddress(); //GPU仮想アドレス
	instanceVertBuffView.SizeInBytes = sizeof(Vector3) * 50000; //頂点バッファのサイズ
	instanceVertBuffView.StrideInBytes = sizeof(Vector3); //頂点一個のサイズ

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);

	viewProjectionBuffT->matrix = matProjection;

	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
	PostEffect::InitPipeline();
	hoge.SetTexture("RenderTargetTex_hoge");
}

void MainTestScene::Update()
{
	light.Update();
	camera.Update();

	sphere.TransferBuffer(camera.viewProjection);
	sphere2.TransferBuffer(camera.viewProjection);
	sprite.TransferBuffer();
	sprite2.TransferBuffer();
	hoge.TransferBuffer();
}

void MainTestScene::Draw()
{
	sphere.Draw();
	sphere2.Draw();

	sprite.Draw();
	sprite2.Draw();

	for (int i = 0; i < 10000; i++) {
		SimpleDrawer::DrawCircle(Util::GetRand(0, RWindow::GetWidth()), Util::GetRand(0, RWindow::GetHeight()), 5, Color(1, 0, 1, 1));
	}

	RenderOrder order;
	order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	order.vertView = &vertBuffView;
	order.instanceVertView = &instanceVertBuffView;
	order.indexCount = 3;
	order.instanceCount = 5000;
	order.rootSignature = testRootSignature.ptr.Get();
	order.pipelineState = testPipeline.ptr.Get();
	order.rootData = {
		{RootDataType::CBV, viewProjectionBuffT.buff.GetGPUVirtualAddress()}
	};

	//Renderer::DrawCall("Sprite", order);

	/*Renderer::SetToBackBuffer();
	Renderer::SetRootSignature(PostEffect::rootSignature);
	Renderer::SetPipeline(PostEffect::pipelineState);
	hoge.TransferBuffer();
	hoge.DrawCommands();

	/*std::vector<RootData> rootData{
		{TextureManager::Get(hoge.texture).gpuHandle},
		{D3D12_ROOT_PARAMETER_TYPE_CBV, hoge.materialBuff.constBuff->GetGPUVirtualAddress()}
	};
	Renderer::DrawCall("TestRenderStage", &hoge.vertBuff.view, &hoge.indexBuff.view, 6, rootData);*/
}