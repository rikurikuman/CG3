#include <TimeManager.h>
#include "SpherePolygonScene.h"
#include <RImGui.h>
#include <Quaternion.h>

SpherePolygonScene::SpherePolygonScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.transform.position = { 0, 5, 0 };
	sphere.transform.UpdateMatrix();

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

void SpherePolygonScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void SpherePolygonScene::Update()
{
	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 260 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			sphere.transform.position = { 0, 5, 0 };
			sphere.transform.scale = { 1, 1, 1 };
			radius = 1;

			posA = { 0, 0, 1 };
			posB = { -1, 0, -1 };
			posC = { 1, 0, -1 };
		}

		ImGui::Text("Sphere");

		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position", &sphere.transform.position.x, 0.01f);
		ImGui::DragFloat("Scale", &radius, 0.01f);

		ImGui::Text("Polygon");
		ImGui::DragFloat3("Position1", &posA.x, 0.01f);
		ImGui::DragFloat3("Position2", &posB.x, 0.01f);
		ImGui::DragFloat3("Position3", &posC.x, 0.01f);

		ImGui::End();
	}

	if (autoMove) {
		sphere.transform.position.y += 5.0f * TimeManager::deltaTime * moveDir;
		if (sphere.transform.position.y > 5
			|| sphere.transform.position.y < 0) {
			sphere.transform.position.y = Util::Clamp(sphere.transform.position.y, 0.0f, 5.0f);
			moveDir *= -1;
		}
	}

	colSphere.pos = sphere.transform.position;
	colSphere.r = radius;
	sphere.transform.scale = { radius, radius, radius };
	sphere.transform.UpdateMatrix();

	colPolygon.p0 = posA;
	colPolygon.p1 = posC;
	colPolygon.p2 = posB;

	if (ColPrimitive3D::CheckSphereToTriangle(colSphere, colPolygon)) {
		sphere.tuneMaterial.color = { 1, 0, 0, 1 };
	}
	else {
		sphere.tuneMaterial.color = { 1, 1, 1, 1 };
	}

	light.Update();
	sphere.TransferBuffer(Camera::nowCamera->viewProjection);

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

void SpherePolygonScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());

	sphere.DrawCommands();
	
	RDirectX::GetCommandList()->SetPipelineState(polygonPipeline.ptr.Get());

	//頂点バッファビューの設定コマンド
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &vertBuff.view);

	//定数バッファビューの設定コマンド
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(4, LightGroup::nowLight->buffer.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする
	RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get("").gpuHandle);

	//描画コマンド
	RDirectX::GetCommandList()->DrawInstanced(3, 1, 0, 0); // 全ての頂点を使って描画
}
