#include "SimpleDrawer.h"
#include "RDirectX.h"
#include "RWindow.h"
#include "TextDrawer.h"
#include <chrono>

using namespace std;

void SimpleDrawer::DrawBox(float x1, float y1, float x2, float y2, Color color, bool fillFlag)
{
	SimpleDrawer* instance = GetInstance();

	shared_ptr<DrawBoxInfo> info = make_shared<DrawBoxInfo>();
	info->start = { x1, y1 };
	info->end = { x2, y2 };

	//頂点データ
	VertexP vertices[] = {
		{{ x1, y1, 0.0f }}, //左上
		{{ x1, y2, 0.0f }}, //左下
		{{ x2, y1, 0.0f }}, //右上
		{{ x2, y2, 0.0f }}, //右下
	};

	//頂点インデックスデータ
	UINT indices[] = {
		0, 2, 1,
		1, 2, 3
	};

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);

	info->vertBuff.Init(vertices, _countof(vertices));
	info->indexBuff.Init(indices, _countof(indices));
	*info->colorBuff.constMap = color;
	info->viewProjectionBuff.constMap->matrix = matProjection;

	instance->infoList.push_back(info);
	
	info->pipelineState = instance->pipelineState;
	if (fillFlag) {
		info->pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	}
	else {
		info->pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	info->pipelineState.Create();

	RDirectX::GetCommandList()->SetPipelineState(info->pipelineState.ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(instance->rootSignature.ptr.Get());

	//頂点バッファビューの設定コマンド
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &info->vertBuff.view);

	//インデックスバッファビューの設定コマンド
	RDirectX::GetCommandList()->IASetIndexBuffer(&info->indexBuff.view);

	//定数バッファビューの設定コマンド
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(0, info->colorBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, info->viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//描画コマンド
	RDirectX::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}

void SimpleDrawer::DrawString(float x, float y, std::string text, Color color, std::string fontTypeFace, UINT fontSize, Vector2 anchor)
{
	SimpleDrawer* instance = GetInstance();
	shared_ptr<DrawStringInfo> info = make_shared<DrawStringInfo>();
	TextureHandle tex = TextDrawer::CreateStringTexture(text, fontTypeFace, fontSize);
	info->sprite = Sprite(tex, anchor);
	info->sprite.transform.position = { x, y, 0 };
	info->sprite.transform.UpdateMatrix();
	info->sprite.material.color = color;
	info->sprite.TransferBuffer();

	info->sprite.Draw();

	TextureManager::UnRegisterAtEndFrame(tex);
	instance->infoList.push_back(info);
}

void SimpleDrawer::Init()
{
	rootSignature = RDirectX::GetDefRootSignature();

	// ルートパラメータの設定
	RootParamaters rootParams(2);
	//定数バッファ0番(Color)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ2番(ViewProjection)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	rootSignature.desc.RootParamaters = rootParams;
	rootSignature.Create();

	pipelineState = RDirectX::GetDefPipeline();

	pipelineState.desc.VS = Shader("./Shader/SimpleVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/SimplePS.hlsl", "main", "ps_5_0");

	// ラスタライザの設定
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;

	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();

	//DrawString用
	rootSignatureForString = SpriteManager::GetInstance()->GetRootSignature();

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //リニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える
	rootSignatureForString.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignatureForString.Create();

	pipelineStateForString = SpriteManager::GetInstance()->GetGraphicsPipeline();
	pipelineStateForString.desc.pRootSignature = rootSignatureForString.ptr.Get();
	pipelineStateForString.Create();
}
