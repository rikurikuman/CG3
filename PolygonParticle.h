#pragma once
#include "RConstBuffer.h"
#include "ViewProjection.h"
#include "Vector2.h"
#include "Material.h"
#include "Transform.h"
#include "Vertex.h"

class PolygonParticle
{
public:
	VertexPNU vertices[3];
	Material material;

	Transform transform;
	Vector3 velocity;
	Vector3 accel;

	float aliveTime = 0;
	float maxAliveTime = 0;

	bool alive = false;

	RootSignature* root = nullptr;
	GraphicsPipeline* pipeline = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	VertexPNU* vertMap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertBuffView{};

	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	PolygonParticle(RootSignature* root, GraphicsPipeline* pipeline);

	void Update();
	void DrawCommands();
};

