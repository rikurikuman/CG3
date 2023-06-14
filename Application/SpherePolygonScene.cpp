#include <TimeManager.h>
#include "SpherePolygonScene.h"
#include <RImGui.h>
#include <Quaternion.h>
#include <Renderer.h>

SpherePolygonScene::SpherePolygonScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.mTransform.position = { 0, 5, 0 };
	sphere.mTransform.UpdateMatrix();

	polygonPipeline = RDirectX::GetDefPipeline();
	polygonPipeline.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	polygonPipeline.Create();

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Init(verts, 3);

	camera.mViewProjection.mEye = { 0, 3, -10 };
	camera.mViewProjection.mTarget = { 0, 3, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void SpherePolygonScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void SpherePolygonScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 260 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			sphere.mTransform.position = { 0, 5, 0 };
			sphere.mTransform.scale = { 1, 1, 1 };
			radius = 1;

			posA = { 0, 0, 1 };
			posB = { -1, 0, -1 };
			posC = { 1, 0, -1 };
		}

		ImGui::Text("Sphere");

		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position", &sphere.mTransform.position.x, 0.01f);
		ImGui::DragFloat("Scale", &radius, 0.01f);

		ImGui::Text("Polygon");
		ImGui::DragFloat3("Position1", &posA.x, 0.01f);
		ImGui::DragFloat3("Position2", &posB.x, 0.01f);
		ImGui::DragFloat3("Position3", &posC.x, 0.01f);

		ImGui::End();
	}

	if (autoMove) {
		sphere.mTransform.position.y += 5.0f * TimeManager::deltaTime * moveDir;
		if (sphere.mTransform.position.y > 5
			|| sphere.mTransform.position.y < 0) {
			sphere.mTransform.position.y = Util::Clamp(sphere.mTransform.position.y, 0.0f, 5.0f);
			moveDir *= -1;
		}
	}

	colSphere.pos = sphere.mTransform.position;
	colSphere.r = radius;
	sphere.mTransform.scale = { radius, radius, radius };
	sphere.mTransform.UpdateMatrix();

	colPolygon.p0 = posA;
	colPolygon.p1 = posC;
	colPolygon.p2 = posB;

	if (ColPrimitive3D::CheckSphereToTriangle(colSphere, colPolygon)) {
		sphere.mTuneMaterial.mColor = { 1, 0, 0, 1 };
	}
	else {
		sphere.mTuneMaterial.mColor = { 1, 1, 1, 1 };
	}

	light.Update();
	sphere.TransferBuffer(Camera::sNowCamera->mViewProjection);

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Update(verts, 3);
	transformBuff.mConstMap->matrix = Matrix4();
	materialBuff.mConstMap->color = { 1, 1, 1, 1 };
	materialBuff.mConstMap->ambient = { 1, 1, 1 };
	Camera::sNowCamera->mViewProjection.Transfer(viewProjectionBuff.mConstMap);
}

void SpherePolygonScene::Draw()
{
	sphere.Draw();
	
	RenderOrder polygon;
	polygon.pipelineState = polygonPipeline.mPtr.Get();
	polygon.rootData = {
		{TextureManager::Get("").mGpuHandle},
		{RootDataType::CBV, materialBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, transformBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, viewProjectionBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, LightGroup::sNowLight->mBuffer.mConstBuff->GetGPUVirtualAddress()},
	};
	polygon.vertBuff = vertBuff;
	polygon.indexCount = 3;

	Renderer::DrawCall("Opaque", polygon);
}
