#include "SimpleDrawer.h"
#include "RDirectX.h"
#include "RWindow.h"
#include "TextDrawer.h"
#include <chrono>
#include <Renderer.h>

using namespace std;

void SimpleDrawer::DrawCircle(int x, int y, int r, Color color) {
	return;
	SimpleDrawer* instance = GetInstance();
	shared_ptr<DrawCircleInfo> info = make_shared<DrawCircleInfo>();

	std::vector<VertexP> vertices;
	std::vector<UINT> indices;
	
	const auto& vertBuffItr = instance->recycleBuffs.find("Circle" + to_string(r) + "Vert");
	const auto& indexBuffItr = instance->recycleBuffs.find("Circle" + to_string(r) + "Index");

	if (vertBuffItr == instance->recycleBuffs.end() || indexBuffItr == instance->recycleBuffs.end()) {
		int cx = 0;
		int cy = r;
		int d = 1 - r;
		int dH = 3;
		int dD = 5 - 2 * r;

		for (cx = 0; cx <= cy; cx++) {
			if (d < 0) {
				d += dH;
				dH += 2;
				dD += 2;
			}
			else {
				d += dD;
				dH += 2;
				dD += 4;
				--cy;
			}

			vertices.push_back(VertexP({ static_cast<float>(cy), static_cast<float>(cx), 0}));
			vertices.push_back(VertexP({ static_cast<float>(cx), static_cast<float>(cy), 0 }));
			vertices.push_back(VertexP({ static_cast<float>(-cx), static_cast<float>(cy), 0 }));
			vertices.push_back(VertexP({ static_cast<float>(-cy), static_cast<float>(cx), 0 }));
			vertices.push_back(VertexP({ static_cast<float>(-cy), static_cast<float>(-cx), 0 }));
			vertices.push_back(VertexP({ static_cast<float>(-cx), static_cast<float>(-cy), 0 }));
			vertices.push_back(VertexP({ static_cast<float>(cx), static_cast<float>(-cy), 0 }));
			vertices.push_back(VertexP({ static_cast<float>(cy), static_cast<float>(-cx), 0 }));
		}
		vertices.push_back(VertexP({ 0, 0, 0 }));

		for (int i = 0; i < 8; i++) {
			UINT index = i;
			UINT count = 0;
			//if (i % 2 != 0) {
			//	index = static_cast<UINT>(vertices.size() - 8 + i);
			//	while (count < vertices.size() / 8) {
			//		indices.push_back(index);
			//		index -= 8;
			//		count++;
			//	}
			//}

			while (count < vertices.size() / 8 - 1) {
				indices.push_back(index);
				indices.push_back(index + 8);
				indices.push_back(static_cast<UINT>(vertices.size() - 1));
				index += 8;
				count++;
			}
		}
		indices.push_back(0);
	}

	std::shared_ptr<SRBufferPtr> vertBuffT;
	if (vertBuffItr == instance->recycleBuffs.end()) {
		vertBuffT = std::make_shared<SRBufferPtr>(SRBufferAllocator::Alloc(sizeof(VertexP) * vertices.size(), 1));
		VertexP* vertMap = reinterpret_cast<VertexP*>(vertBuffT->Get());
		for (int i = 0; i < vertices.size(); i++) {
			vertMap[i] = vertices[i];
		}
		instance->recycleBuffs["Circle" + to_string(r) + "Vert"] = vertBuffT;
	}
	else {
		vertBuffT = vertBuffItr->second;
	}
	
	info->vertView.BufferLocation = vertBuffT->GetGPUVirtualAddress(); //GPU仮想アドレス
	info->vertView.SizeInBytes = static_cast<UINT>(sizeof(VertexP) * vertBuffT->GetRegionPtr()->region->size / sizeof(VertexP)); //頂点バッファのサイズ
	info->vertView.StrideInBytes = sizeof(VertexP); //頂点一個のサイズ
	
	std::shared_ptr<SRBufferPtr> indexBuffT;
	if (indexBuffItr == instance->recycleBuffs.end()) {
		indexBuffT = std::make_shared<SRBufferPtr>(SRBufferAllocator::Alloc(sizeof(UINT) * indices.size(), 1));
		UINT* indexMap = reinterpret_cast<UINT*>(indexBuffT->Get());
		for (int i = 0; i < indices.size(); i++) {
			indexMap[i] = indices[i];
		}
		instance->recycleBuffs["Circle" + to_string(r) + "Index"] = indexBuffT;
	}
	else {
		indexBuffT = indexBuffItr->second;
	}
	info->indexView.BufferLocation = indexBuffT->GetGPUVirtualAddress();
	info->indexView.Format = DXGI_FORMAT_R32_UINT;
	info->indexView.SizeInBytes = static_cast<UINT>(sizeof(UINT) * indexBuffT->GetRegionPtr()->region->size / sizeof(UINT));

	info->indexCount = static_cast<UINT>(indexBuffT->GetRegionPtr()->region->size / sizeof(UINT));

	info->pos = Vector2(static_cast<float>(x), static_cast<float>(y));
	info->color = color;
	*info->colorBuff.Get() = color;

	instance->infoList.push_back(info);
	instance->circleInstance["Circle" + to_string(r)].push_back(info);

	//RenderOrder order;
	//order.rootSignature = instance->rootSignatureForCircle.ptr.Get();
	//order.pipelineState = instance->pipelineStateForCircle.ptr.Get();
	//order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	//order.vertView = &info->vertView;
	//order.indexView = &info->indexView;
	//order.indexCount = static_cast<UINT>(indexBuffT->GetRegionPtr()->region->size / sizeof(UINT));
	//order.rootData = {
	//	{RootDataType::CBV, info->colorBuff.buff.GetGPUVirtualAddress()},
	//	{RootDataType::CBV, instance->viewProjectionBuff.buff.GetGPUVirtualAddress()},
	//};
	//Renderer::DrawCall("Sprite", order);
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

void SimpleDrawer::DrawInstancing() {
	SimpleDrawer* instance = GetInstance();

	if (!instance->circleInstance.empty()) {
		struct CircleInstance {
			Vector2 pos;
			Color color;
		};

		for (auto itr = instance->circleInstance.begin(); itr != instance->circleInstance.end(); itr++) {
			std::list<std::shared_ptr<DrawCircleInfo>>& infoList = itr->second;

			std::shared_ptr<SRBufferPtr> instanceVertBuff = std::make_shared<SRBufferPtr>(SRBufferAllocator::Alloc(sizeof(CircleInstance) * infoList.size(), 1));
			CircleInstance* indexMap = reinterpret_cast<CircleInstance*>(instanceVertBuff->Get());

			UINT count = 0;
			for (std::shared_ptr<DrawCircleInfo>& info : infoList) {
				CircleInstance inst = { info->pos, info->color };
				indexMap[count] = inst;
				count++;
			}

			instance->recycleBuffs[itr->first + "Instance"] = instanceVertBuff;

			const auto& vertBuffItr = instance->recycleBuffs.find(itr->first + "Vert");
			const auto& indexBuffItr = instance->recycleBuffs.find(itr->first + "Index");

			D3D12_VERTEX_BUFFER_VIEW instanceVertBuffView{};
			instanceVertBuffView.BufferLocation = instanceVertBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
			instanceVertBuffView.SizeInBytes = static_cast<UINT>(sizeof(CircleInstance) * infoList.size()); //頂点バッファのサイズ
			instanceVertBuffView.StrideInBytes = static_cast<UINT>(sizeof(CircleInstance)); //頂点一個のサイズ
			instance->test[itr->first] = instanceVertBuffView;

			RenderOrder order;
			order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			order.vertView = &infoList.begin()->get()->vertView;
			order.indexView = &infoList.begin()->get()->indexView;
			order.instanceVertView = &instance->test[itr->first];
			order.indexCount = infoList.begin()->get()->indexCount;
			order.instanceCount = static_cast<UINT>(infoList.size());
			order.rootSignature = instance->rootSignatureForCircle.ptr.Get();
			order.pipelineState = instance->pipelineStateForCircle.ptr.Get();
			order.rootData = {
				{RootDataType::CBV, instance->viewProjectionBuff.buff.GetGPUVirtualAddress()}
			};

			Renderer::DrawCall("Sprite", order);
		}
	}
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

	//DrawCircle用
	RootParamaters rootParamCircle(1);
	rootParamCircle[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParamCircle[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParamCircle[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParamCircle[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	rootSignatureForCircle.desc.RootParamaters = rootParamCircle;
	rootSignatureForCircle.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureForCircle.Create();

	pipelineStateForCircle = pipelineState;
	pipelineStateForCircle.desc.VS = Shader("Shader/SimpleInstanceVS.hlsl", "main", "vs_5_0");
	pipelineStateForCircle.desc.PS = Shader("Shader/SimpleInstancePS.hlsl", "main", "ps_5_0");
	pipelineStateForCircle.desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"POSITION", 1, DXGI_FORMAT_R32G32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		},
	};
	pipelineStateForCircle.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateForCircle.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineStateForCircle.desc.pRootSignature = rootSignatureForCircle.ptr.Get();
	pipelineStateForCircle.Create();

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

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);

	viewProjectionBuff->matrix = matProjection;
}
