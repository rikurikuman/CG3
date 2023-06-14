#include <TimeManager.h>
#include "RayPolygonScene.h"
#include <RImGui.h>
#include <Quaternion.h>
#include <Renderer.h>

RayPolygonScene::RayPolygonScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));

	sphere.mTransform.position = { 0, 5, 0 };
	sphere.mTransform.scale = { 0.1f, 0.1f, 0.1f };
	sphere.mTransform.UpdateMatrix();

	colRay = { {0, 5, 0}, {0, -1, 0} };

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

void RayPolygonScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void RayPolygonScene::Update()
{
	//ƒeƒXƒgGUI
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

	sphere.mTransform.position = colRay.start;
	sphere.mTransform.UpdateMatrix();

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
		sphere.mTuneMaterial.mColor = { 1, 0, 0, 1 };
		ray.mTuneMaterial.mColor = { 0, 0, 1, 1 };
		ray.mTransform.position = colRay.start;
		ray.mTransform.position += colRay.dir * dis / 2.0f;
		ray.mTransform.scale = { 0.1f, dis, 0.1f };
	}
	else {
		sphere.mTuneMaterial.mColor = { 1, 1, 1, 1 };
		ray.mTuneMaterial.mColor = { 1, 1, 1, 1 };
		ray.mTransform.position = colRay.start;
		ray.mTransform.position += colRay.dir * 50;
		ray.mTransform.scale = { 0.1f, 100, 0.1f };
	}
	ray.mTransform.UpdateMatrix();

	light.Update();
	sphere.TransferBuffer(Camera::sNowCamera->mViewProjection);
	ray.TransferBuffer(Camera::sNowCamera->mViewProjection);

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Update(verts, 3);

	transformBuff.mConstMap->matrix = Matrix4();
	materialBuff.mConstMap->color = { 1, 1, 1, 1 };
	materialBuff.mConstMap->ambient = { 1, 1, 1 };
	materialBuffT->color = { 1, 1, 1, 1 };
	materialBuffT->ambient = { 1, 1, 1 };
	Camera::sNowCamera->mViewProjection.Transfer(viewProjectionBuff.mConstMap);
	Camera::sNowCamera->mViewProjection.Transfer(viewProjectionBuffT.Get());
}

void RayPolygonScene::Draw()
{
	sphere.Draw();
	ray.Draw();
	
	RenderOrder polygon;
	polygon.pipelineState = polygonPipeline.mPtr.Get();
	polygon.rootData = {
		{TextureManager::Get("").mGpuHandle},
		{RootDataType::CBV, materialBuffT.mBuff.GetGPUVirtualAddress()},
		{RootDataType::CBV, transformBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, viewProjectionBuffT.mBuff.GetGPUVirtualAddress()},
		{RootDataType::CBV, LightGroup::sNowLight->mBuffer.mConstBuff->GetGPUVirtualAddress()},
	};
	polygon.vertBuff = vertBuff;
	polygon.indexCount = 3;

	Renderer::DrawCall("Opaque", polygon);
}
