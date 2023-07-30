#include "DissolveModel.h"

void DissolveModel::TransferBuffer(ViewProjection viewprojection)
{
	int32_t count = 0;
	for (std::shared_ptr<ModelMesh> data : mModel->mData) {
		std::string name = data->mMaterial.mName;
		if (name.empty()) {
			name = "NoNameMaterial_" + count;
			data->mMaterial.mName = name;
			count++;
		}
		Material mat = data->mMaterial;
		mat.mColor.r *= mTuneMaterial.mColor.r;
		mat.mColor.g *= mTuneMaterial.mColor.g;
		mat.mColor.b *= mTuneMaterial.mColor.b;
		mat.mColor.a *= mTuneMaterial.mColor.a;
		mat.mAmbient *= mTuneMaterial.mAmbient;
		mat.mDiffuse *= mTuneMaterial.mDiffuse;
		mat.mSpecular *= mTuneMaterial.mSpecular;
		mat.Transfer(mMaterialBuffMap[name].Get());

		Material matD = data->mMaterial;
		matD.mColor.r *= mTuneMaterialD.mColor.r;
		matD.mColor.g *= mTuneMaterialD.mColor.g;
		matD.mColor.b *= mTuneMaterialD.mColor.b;
		matD.mColor.a *= mTuneMaterialD.mColor.a;
		matD.mAmbient *= mTuneMaterialD.mAmbient;
		matD.mDiffuse *= mTuneMaterialD.mDiffuse;
		matD.mSpecular *= mTuneMaterialD.mSpecular;
		matD.Transfer(mMaterialBuffDMap[name].Get());
	}

	mTransform.Transfer(mTransformBuff.Get());
	viewprojection.Transfer(mViewProjectionBuff.Get());
	*dissolveDataBuff.Get() = dataDissolve;
}

void DissolveModel::Draw()
{
	for (std::shared_ptr<ModelMesh> data : mModel->mData) {
		RenderOrder order;

		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT },
			{ TextureManager::Get(dissolveTex).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, dissolveDataBuff.mBuff },
		};
		if (!mTuneMaterial.mTexture.empty()) {
			rootData[0] = { TextureManager::Get(mTuneMaterial.mTexture).mGpuHandle };
		}

		order.rootData = rootData;
		order.vertView = &data->mVertBuff.mView;
		order.indexView = &data->mIndexBuff.mView;
		order.indexCount = static_cast<uint32_t>(data->mIndices.size());
		order.mRootSignature = GetRootSignature().mPtr.Get();
		order.pipelineState = GetGraphicsPipelineA().mPtr.Get();

		std::string stage = "Opaque";
		if (data->mMaterial.mColor.a < 1.0f || mTuneMaterial.mColor.a < 1.0f) stage = "Transparent";
		Renderer::DrawCall(stage, order);

		if (true) {
			RenderOrder orderB = order;

			order.rootData[0] = { TextureManager::Get("").mGpuHandle};
			order.rootData[1] = { RootDataType::SRBUFFER_CBV, mMaterialBuffDMap[data->mMaterial.mName].mBuff };
			order.pipelineState = GetGraphicsPipelineB().mPtr.Get();

			std::string stageB = "Opaque";
			if (data->mMaterial.mColor.a < 1.0f || mTuneMaterial.mColor.a < 1.0f) stageB = "Transparent";
			Renderer::DrawCall(stageB, order);
		}
	}
}

RootSignature& DissolveModel::GetRootSignature()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	DescriptorRange descriptorRangeD{};
	descriptorRangeD.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRangeD.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeD.BaseShaderRegister = 1; //�e�N�X�`�����W�X�^1��
	descriptorRangeD.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	RootParamaters rootParams(7);
	//�e�N�X�`�����W�X�^0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@0��(Material)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[1].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[1].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@1��(Transform)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[2].Descriptor.ShaderRegister = 1; //�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@2��(ViewProjection)
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[3].Descriptor.ShaderRegister = 2; //�萔�o�b�t�@�ԍ�
	rootParams[3].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@3��(Light)
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[4].Descriptor.ShaderRegister = 3; //�萔�o�b�t�@�ԍ�
	rootParams[4].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�e�N�X�`�����W�X�^1��
	rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[5].DescriptorTable = DescriptorRanges{ descriptorRangeD };
	rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@4��(Dissolve)
	rootParams[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[6].Descriptor.ShaderRegister = 4; //�萔�o�b�t�@�ԍ�
	rootParams[6].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
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

	return RootSignature::GetOrCreate("DissolveModel", desc);
}

GraphicsPipeline& DissolveModel::GetGraphicsPipelineA()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.VS = Shader::GetOrCreate("DissolveModelA_VS", "Shader/DissolveObjVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("DissolveModelA_PS", "Shader/DissolveObjPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("DissolveModelA", desc);
}

GraphicsPipeline& DissolveModel::GetGraphicsPipelineB()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;

	desc.VS = Shader::GetOrCreate("DissolveModelB_VS", "Shader/DissolveObjVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("DissolveModelB_PS", "Shader/DissolveObjPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("DissolveModelB", desc);
}
