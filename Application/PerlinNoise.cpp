#include "PerlinNoise.h"
#include <RDirectX.h>

PerlinNoise::PerlinNoise()
{
	//���_�f�[�^
	VertexPU vertices[] = {
		{{ -1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, //����
		{{ -1.0f, +1.0f, 0.0f}, {0.0f, 0.0f}}, //����
		{{ +1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, //�E��
		{{ +1.0f, +1.0f, 0.0f}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	mVertBuff.Init(vertices, _countof(vertices));
	mIndexBuff.Init(indices, _countof(indices));

	renderTex = RenderTarget::CreateRenderTexture(128, 128, 0x000000, "");
}

void PerlinNoise::Render()
{
	noiseData.t += 0.001f;
	*noiseDataBuff.Get() = noiseData;

	renderTex->ClearRenderTarget();
	renderTex->ClearDepthStencil();

	RenderOrder orderA;
	orderA.rootData = {
		{RootDataType::SRBUFFER_CBV, noiseDataBuff.mBuff }
	};
	orderA.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	orderA.vertBuff = mVertBuff;
	orderA.indexBuff = mIndexBuff;
	orderA.indexCount = mIndexBuff.GetIndexCount();
	orderA.anchorPoint = Vector3(0, 0, 0);
	orderA.mRootSignature = GetRootSignature().mPtr.Get();
	orderA.pipelineState = GetGraphicsPipeline().mPtr.Get();
	orderA.viewports = {
		{1280, 720, 0, 0, 0.0f, 1.0f},
		{1280, 720, 0, 0, 0.0f, 1.0f}
	};
	orderA.scissorRects = {
		{0, 1280, 0, 720},
		{0, 1280, 0, 720}
	};
	orderA.renderTargets = { renderTex->mName };
	Renderer::DrawCall("Opaque", orderA);
}

RootSignature& PerlinNoise::GetRootSignature()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	RootParamaters rootParams(1);
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	desc.RootParamaters = rootParams;

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //���j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f; //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_�[���炾��������
	desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };

	return RootSignature::GetOrCreate("PerlinNoise", desc);
}

GraphicsPipeline& PerlinNoise::GetGraphicsPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.InputLayout = {
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
	};

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	desc.VS = Shader::GetOrCreate("PerlinNoise_VS", "Shader/PerlinNoiseVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("PerlinNoise_PS", "Shader/PerlinNoisePS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("PerlinNoise", desc);
}
