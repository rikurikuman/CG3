#include "ModelObj.h"

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	transform.Transfer(transformBuff.constMap);
	viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::DrawCommands()
{
	for (std::shared_ptr<ModelData> data : model->data) {
		//�p�C�v���C���Z�b�g
		RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

		//���_�o�b�t�@�r���[�̐ݒ�R�}���h
		RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &data->vertexBuff.view);

		//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
		RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&data->indexBuff.view);

		//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, data->materialBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

		//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����
		RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(data->material.texture).gpuHandle);

		//�`��R�}���h
		RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(static_cast<UINT>(data->indices.size()), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
	}
}
