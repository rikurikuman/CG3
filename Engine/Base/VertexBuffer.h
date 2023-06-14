#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Vertex.h"

class VertexBuffer
{
public:
	VertexBuffer() {};

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	VertexBuffer(VertexP* list, uint32_t size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	VertexBuffer(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	VertexBuffer(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	VertexBuffer(std::vector<VertexPNU> list);

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	void Init(VertexP* list, uint32_t size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	void Init(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	void Init(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	void Init(std::vector<VertexPNU> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを更新する
	void Update(VertexPNU* list, uint32_t size);

	Microsoft::WRL::ComPtr<ID3D12Resource> buff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW view{};
};

