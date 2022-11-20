#pragma once
#include "RConstBuffer.h"
#include "ViewProjection.h"
#include "Vector2.h"

class Particle
{
public:
	struct VertexInfo {
		Vector3 pos;
		Vector2 scale;
		Color color;
	};

	VertexInfo data;
	Vector2 scale;
	Color color;
	Color endcolor;
	Vector3 velocity;
	Vector3 accel;
	
	TextureHandle texture = TextureManager::Load("./Resources/effect.png", "Particle");
	float aliveTime = 0;
	float maxAliveTime = 0;

	bool alive = false;

	RootSignature* root = nullptr;
	GraphicsPipeline* pipeline = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	VertexInfo* vertMap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertBuffView{};

	RConstBuffer<Matrix4> billboardBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Particle(RootSignature* root, GraphicsPipeline* pipeline);

	void Update();
	void DrawCommands();
};

