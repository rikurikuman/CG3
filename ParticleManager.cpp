#include "ParticleManager.h"
#include "ModelObj.h"

ParticleManager::ParticleManager() {
	normalParticleRoot = RDirectX::GetInstance()->rootSignature;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	RootParamaters rootParams(3);
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	normalParticleRoot.desc.RootParamaters = rootParams;
	normalParticleRoot.Create();

	normalParticlePipeline = RDirectX::GetInstance()->pipelineState;
	normalParticlePipeline.desc.VS = Shader("Shader/TestVS.hlsl", "main", "vs_5_0");
	normalParticlePipeline.desc.GS = Shader("Shader/TestGS.hlsl", "main", "gs_5_0");
	normalParticlePipeline.desc.PS = Shader("Shader/TestPS.hlsl", "main", "ps_5_0");

	normalParticlePipeline.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	normalParticlePipeline.desc.BlendState.AlphaToCoverageEnable = false;

	InputLayout gridInputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = normalParticlePipeline.desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	normalParticlePipeline.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	normalParticlePipeline.desc.InputLayout = gridInputLayout;
	normalParticlePipeline.desc.pRootSignature = normalParticleRoot.ptr.Get();
	normalParticlePipeline.Create();

	subParticlePipeline = normalParticlePipeline;
	D3D12_RENDER_TARGET_BLEND_DESC& subblenddesc = subParticlePipeline.desc.BlendState.RenderTarget[0];
	subblenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	subParticlePipeline.Create();


	polygonParticleRoot = RDirectX::GetInstance()->rootSignature;
	polygonParticlePipeline = RDirectX::GetInstance()->pipelineState;
	polygonParticlePipeline.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //背面カリングしない
	polygonParticlePipeline.Create();

	for (int i = 0; i < 1024; i++) {
		normalParticlePool.emplace_back(&normalParticleRoot, &normalParticlePipeline);
	}

	for (int i = 0; i < 1024; i++) {
		polygonParticlePool.emplace_back(&polygonParticleRoot, &polygonParticlePipeline);
	}
}

void ParticleManager::Update()
{
	for (auto itr = normalParticlePool.begin(); itr != normalParticlePool.end(); itr++) {
		Particle& pat = *itr;
		if (pat.alive) {
			pat.Update();
		}
	}

	for (auto itr = polygonParticlePool.begin(); itr != polygonParticlePool.end(); itr++) {
		PolygonParticle& pat = *itr;
		if (pat.alive) {
			pat.Update();
		}
	}
}

void ParticleManager::Draw()
{
	for (auto itr = normalParticlePool.begin(); itr != normalParticlePool.end(); itr++) {
		Particle& pat = *itr;
		if (pat.alive) {
			pat.DrawCommands();
		}
	}

	for (auto itr = polygonParticlePool.begin(); itr != polygonParticlePool.end(); itr++) {
		PolygonParticle& pat = *itr;
		if (pat.alive) {
			pat.DrawCommands();
		}
	}
}

void ParticleManager::SpawnNormalParticle(Vector3 pos, Vector3 velocity, Vector3 accel, Vector2 scale, Color color, float aliveTime, Color endcolor, bool subtract)
{
	GraphicsPipeline* pipeline = &normalParticlePipeline;
	if (subtract) {
		pipeline = &subParticlePipeline;
	}

	for (auto itr = normalParticlePool.begin(); itr != normalParticlePool.end(); itr++) {
		Particle& pat = *itr;
		if (!pat.alive) {
			pat.data.pos = pos;
			pat.scale = scale;
			pat.color = color;
			pat.endcolor = endcolor;
			pat.velocity = velocity;
			pat.accel = accel;
			pat.aliveTime = aliveTime;
			pat.maxAliveTime = aliveTime;
			pat.alive = true;
			return;
		}
	}

	Particle pat = Particle(&normalParticleRoot, pipeline);
	pat.data.pos = pos;
	pat.data.scale = scale;
	pat.data.color = color;
	pat.velocity = velocity;
	pat.accel = accel;
	pat.aliveTime = aliveTime;
	pat.alive = true;
	normalParticlePool.push_back(pat);
}

void ParticleManager::SpawnPolygonExplostion(ModelObj& obj, Vector3 minVelocity, Vector3 maxVelocity, Vector3 minAccel, Vector3 maxAccel, float aliveTime)
{
	for (std::shared_ptr<ModelData> data : obj.model->data) {
		for (unsigned int i = 0; i < data->indices.size() / 3; i++) {
			unsigned int index0 = data->indices[i * 3 + 0];
			unsigned int index1 = data->indices[i * 3 + 1];
			unsigned int index2 = data->indices[i * 3 + 2];

			VertexPNU p0 = data->vertexs[index0];
			VertexPNU p1 = data->vertexs[index1];
			VertexPNU p2 = data->vertexs[index2];

			bool ok = false;
			for (auto itr = polygonParticlePool.begin(); itr != polygonParticlePool.end(); itr++) {
				PolygonParticle& pat = *itr;
				if (!pat.alive) {
					pat.vertices[0] = p0;
					pat.vertices[1] = p1;
					pat.vertices[2] = p2;
					pat.transform = obj.transform;
					pat.material = data->material;
					pat.velocity = {
						Util::GetRand(minVelocity.x, maxVelocity.x),
						Util::GetRand(minVelocity.y, maxVelocity.y),
						Util::GetRand(minVelocity.z, maxVelocity.z),
					};
					pat.accel = {
						Util::GetRand(minAccel.x, maxAccel.x),
						Util::GetRand(minAccel.y, maxAccel.y),
						Util::GetRand(minAccel.z, maxAccel.z),
					};
					pat.aliveTime = aliveTime;
					pat.maxAliveTime = aliveTime;
					pat.alive = true;
					ok = true;
					break;
				}
			}

			if (!ok) {
				PolygonParticle pat = PolygonParticle(&polygonParticleRoot, &polygonParticlePipeline);
				pat.vertices[0] = p0;
				pat.vertices[1] = p1;
				pat.vertices[2] = p2;
				pat.transform = obj.transform;
				pat.material = data->material;
				pat.velocity = {
						Util::GetRand(minVelocity.x, maxVelocity.x),
						Util::GetRand(minVelocity.y, maxVelocity.y),
						Util::GetRand(minVelocity.z, maxVelocity.z),
				};
				pat.accel = {
					Util::GetRand(minAccel.x, maxAccel.x),
					Util::GetRand(minAccel.y, maxAccel.y),
					Util::GetRand(minAccel.z, maxAccel.z),
				};
				pat.aliveTime = aliveTime;
				pat.maxAliveTime = aliveTime;
				pat.alive = true;
				polygonParticlePool.push_back(pat);
			}
		}
	}
}
