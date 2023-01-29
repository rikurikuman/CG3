#include "Camera.h"
#include "Course.h"

Course::Course() {
	courseTerrain = ModelObj(Model::LoadWithAIL("./Resources/Model/pCourse", "pCourse_Terrain.obj", "Testest"));
	Model::LoadWithAIL("./Resources/Model/pCourse", "pCourse_Collision.obj", "CourseCollision");

	for (std::shared_ptr<ModelData> data : ModelManager::Get("CourseCollision")->data) {
		for (UINT i = 0; i < data->indices.size() / 3; i++) {
			UINT index0 = data->indices[i * 3 + 0];
			UINT index1 = data->indices[i * 3 + 1];
			UINT index2 = data->indices[i * 3 + 2];

			Vector3 p0 = data->vertexs[index0].pos;
			Vector3 p1 = data->vertexs[index1].pos;
			Vector3 p2 = data->vertexs[index2].pos;

			Vector3 v1 = p1 - p0;
			Vector3 v2 = p2 - p0;

			Vector3 normal = v1.Cross(v2);
			normal.Normalize();

			courseColMeshes.push_back({ p0, p1, p2, normal });
		}
	}

	checkPoints.push_back({ "goal", {{-15.85f, 0.0f, 0.0f}, {5.0f, 10.0f, 1.0f}}, "check1", true });
	checkPoints.push_back({ "check1", {{-5.5f, 0.0f, 16.0f}, {5.0f, 10.0f, 5.0f}}, "check2", false });
	checkPoints.push_back({ "check2", {{16.0f, 0.0f, 2.0f}, {5.0f, 10.0f, 5.0f}}, "goal", false });
}

void Course::Update()
{
	courseTerrain.TransferBuffer(Camera::nowCamera->viewProjection);
}

void Course::Draw()
{
	courseTerrain.DrawCommands();
}