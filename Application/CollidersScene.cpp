#include "CollidersScene.h"
#include <TimeManager.h>
#include <RImGui.h>
#include <Quaternion.h>

CollidersScene::CollidersScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));

	sphere.transform.position = { 0, 5, 0 };
	sphere.transform.UpdateMatrix();
	sphere2.transform.position = { 2, 3, 0 };
	sphere2.transform.UpdateMatrix();

	colSphere = Colliders::Create<SphereCollider>(Vector3(0, 5, 0), 1.0f);
	colSphere2 = Colliders::Create<SphereCollider>(Vector3(2, 3, 0), 1.0f);
	colPolygon = Colliders::Create<PolygonCollider>(posA, posC, posB);
	colRay = Colliders::Create<RayCollider>(Vector3(0, 7, 0), Vector3(0, -1, 0));
	colRay->onCollision = [&](CollisionInfo info) {
		//念のためチェック
		if (info.hasDistance && info.hasInter) {
			if (info.distance < memDis) {
				memDis = info.distance;
			}
		}
	};

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

void CollidersScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
	Colliders::AllActivate();
}

void CollidersScene::Update()
{
	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 500 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			sphere.transform.position = { 0, 5, 0 };
			sphere.transform.scale = { 1, 1, 1 };
			sphere2.transform.position = { 2, 3, 0 };
			sphere2.transform.scale = { 1, 1, 1 };
			colRay->ray.start = { 0, 7, 0 };
			radiusA = 1;
			radiusB = 1;
			pierce = false;

			posA = { 0, 0, 1 };
			posB = { -1, 0, -1 };
			posC = { 1, 0, -1 };
		}

		ImGui::Text("SphereA");
		ImGui::Checkbox("AutoMove##SphereA", &autoMoveA);
		ImGui::DragFloat3("Position##SphereA", &sphere.transform.position.x, 0.01f);
		ImGui::DragFloat("Scale##SphereA", &radiusA, 0.01f);

		ImGui::Text("SphereB");
		ImGui::Checkbox("AutoMove##SphereB", &autoMoveB);
		ImGui::DragFloat3("Position##SphereB", &sphere2.transform.position.x, 0.01f);
		ImGui::DragFloat("Scale##SphereB", &radiusB, 0.01f);

		ImGui::Text("Ray");
		ImGui::Checkbox("AutoMove##Ray", &autoMoveC);
		ImGui::Checkbox("Pierce", &pierce);
		ImGui::DragFloat3("Position##Ray", &colRay->ray.start.x, 0.01f);

		ImGui::Text("Polygon");
		ImGui::DragFloat3("Position1", &posA.x, 0.01f);
		ImGui::DragFloat3("Position2", &posB.x, 0.01f);
		ImGui::DragFloat3("Position3", &posC.x, 0.01f);

		ImGui::End();
	}

	if (autoMoveA) {
		sphere.transform.position.x += 5.0f * TimeManager::deltaTime * moveDirA;
		if (sphere.transform.position.x > 5
			|| sphere.transform.position.x < -5) {
			sphere.transform.position.x = Util::Clamp(sphere.transform.position.x, -5.0f, 5.0f);
			moveDirA *= -1;
		}
	}

	if (autoMoveB) {
		sphere2.transform.position.x += 5.0f * TimeManager::deltaTime * moveDirB;
		if (sphere2.transform.position.x > 5
			|| sphere2.transform.position.x < -5) {
			sphere2.transform.position.x = Util::Clamp(sphere2.transform.position.x, -5.0f, 5.0f);
			moveDirB *= -1;
		}
	}

	if (autoMoveC) {
		colRay->ray.start.x += 5.0f * TimeManager::deltaTime * moveDirC;
		if (colRay->ray.start.x > 5
			|| colRay->ray.start.x < -5) {
			colRay->ray.start.x = Util::Clamp(colRay->ray.start.x, -5.0f, 5.0f);
			moveDirC *= -1;
		}
	}

	colSphere->sphere.pos = sphere.transform.position;
	colSphere->sphere.r = radiusA;
	colSphere2->sphere.pos = sphere2.transform.position;
	colSphere2->sphere.r = radiusB;

	sphere.transform.scale = { radiusA, radiusA, radiusA };
	sphere.transform.UpdateMatrix();
	sphere2.transform.scale = { radiusB, radiusB, radiusB };
	sphere2.transform.UpdateMatrix();

	colPolygon->polygon.p0 = posA;
	colPolygon->polygon.p1 = posC;
	colPolygon->polygon.p2 = posB;

	colRay->pierce = pierce;

	memDis = FLT_MAX;
	Colliders::Update();

	if (colSphere->HasCollision()) {
		sphere.tuneMaterial.color = { 1, 0, 0, 1 };
	}
	else {
		sphere.tuneMaterial.color = { 1, 1, 1, 1 };
	}

	if (colSphere2->HasCollision()) {
		sphere2.tuneMaterial.color = { 1, 0, 0, 1 };
	}
	else {
		sphere2.tuneMaterial.color = { 1, 1, 1, 1 };
	}

	if (pierce) {
		if (colRay->HasCollision()) {
			ray.tuneMaterial.color = { 0, 0, 1, 1 };
		}
		else {
			ray.tuneMaterial.color = { 1, 1, 1, 1 };
		}
		ray.transform.position = colRay->ray.start;
		ray.transform.position += colRay->ray.dir * 50;
		ray.transform.scale = { 0.1f, 100, 0.1f };
	}
	else {
		if (colRay->HasCollision()) {
			ray.tuneMaterial.color = { 0, 0, 1, 1 };
			ray.transform.position = colRay->ray.start;
			ray.transform.position += colRay->ray.dir * memDis / 2.0f;
			ray.transform.scale = { 0.1f, memDis, 0.1f };
		}
		else {
			ray.tuneMaterial.color = { 1, 1, 1, 1 };
			ray.transform.position = colRay->ray.start;
			ray.transform.position += colRay->ray.dir * 50;
			ray.transform.scale = { 0.1f, 100, 0.1f };
		}
	}
	ray.transform.UpdateMatrix();

	if (colPolygon->HasCollision()) {
		materialBuff.constMap->color = { 1, 0, 0, 1 };
	}
	else {
		materialBuff.constMap->color = { 1, 1, 1, 1 };
	}

	light.Update();
	sphere.TransferBuffer(Camera::nowCamera->viewProjection);
	sphere2.TransferBuffer(Camera::nowCamera->viewProjection);
	ray.TransferBuffer(Camera::nowCamera->viewProjection);

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Update(verts, 3);
	transformBuff.constMap->matrix = Matrix4();
	materialBuff.constMap->ambient = { 1, 1, 1 };
	Camera::nowCamera->viewProjection.Transfer(viewProjectionBuff.constMap);
}

void CollidersScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());

	sphere.DrawCommands();
	sphere2.DrawCommands();
	ray.DrawCommands();

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
