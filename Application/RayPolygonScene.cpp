#include <TimeManager.h>
#include "RayPolygonScene.h"
#include <RImGui.h>
#include <Quaternion.h>

RayPolygonScene::RayPolygonScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));

	sphere.transform.position = { 0, 5, 0 };
	sphere.transform.scale = { 0.1f, 0.1f, 0.1f };
	sphere.transform.UpdateMatrix();

	colRay = { {0, 5, 0}, {0, -1, 0} };

	polygonPipeline = RDirectX::GetDefPipeline();
	polygonPipeline.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	polygonPipeline.Create();

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Init(verts, 3);

	camera.viewProjection.eye = { 0, 3, -10 };
	camera.viewProjection.target = { 0, 3, 0 };
	camera.viewProjection.UpdateMatrix();
}

void RayPolygonScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void RayPolygonScene::Update()
{
	//�e�X�gGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 250 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			colRay.start = { 0, 5, 0 };
			posA = { 0, 0, 1 };
			posB = { -1, 0, -1 };
			posC = { 1, 0, -1 };
		}

		ImGui::Text("Ray");

		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position", &colRay.start.x, 0.01f);

		ImGui::Text("Polygon");
		ImGui::DragFloat3("Position1", &posA.x, 0.01f);
		ImGui::DragFloat3("Position2", &posB.x, 0.01f);
		ImGui::DragFloat3("Position3", &posC.x, 0.01f);

		ImGui::End();
	}

	if (autoMove) {
		colRay.start.x += 5.0f * TimeManager::deltaTime * moveDir;
		if (colRay.start.x > 5
			|| colRay.start.x < -5) {
			colRay.start.x = Util::Clamp(colRay.start.x, -5.0f, 5.0f);
			moveDir *= -1;
		}
	}

	sphere.transform.position = colRay.start;
	sphere.transform.UpdateMatrix();

	colPolygon.p0 = posA;
	colPolygon.p1 = posC;
	colPolygon.p2 = posB;

	Vector3 v1 = posC - posA;
	Vector3 v2 = posB - posC;

	Vector3 normal = v1.Cross(v2);
	normal.Normalize();
	colPolygon.normal = normal;

	float dis = 0;
	Vector3 inter;
	if (ColPrimitive3D::CheckRayToTriangle(colRay, colPolygon, &dis, &inter)) {
		sphere.tuneMaterial.color = { 1, 0, 0, 1 };
		ray.tuneMaterial.color = { 0, 0, 1, 1 };
		ray.transform.position = colRay.start;
		ray.transform.position += colRay.dir * dis / 2.0f;
		ray.transform.scale = { 0.1f, dis, 0.1f };
	}
	else {
		sphere.tuneMaterial.color = { 1, 1, 1, 1 };
		ray.tuneMaterial.color = { 1, 1, 1, 1 };
		ray.transform.position = colRay.start;
		ray.transform.position += colRay.dir * 50;
		ray.transform.scale = { 0.1f, 100, 0.1f };
	}
	ray.transform.UpdateMatrix();

	light.Update();
	sphere.TransferBuffer(Camera::nowCamera->viewProjection);
	ray.TransferBuffer(Camera::nowCamera->viewProjection);

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Update(verts, 3);
	transformBuff.constMap->matrix = Matrix4();
	materialBuff.constMap->color = { 1, 1, 1, 1 };
	materialBuff.constMap->ambient = { 1, 1, 1 };
	Camera::nowCamera->viewProjection.Transfer(viewProjectionBuff.constMap);
}

void RayPolygonScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());

	sphere.DrawCommands();
	ray.DrawCommands();
	
	RDirectX::GetCommandList()->SetPipelineState(polygonPipeline.ptr.Get());

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &vertBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(4, LightGroup::nowLight->buffer.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����
	RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get("").gpuHandle);

	//�`��R�}���h
	RDirectX::GetCommandList()->DrawInstanced(3, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
