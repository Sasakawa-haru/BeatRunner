#pragma once
#include<cstdint>
//BeatRunner 独自モデル形式

struct BRModelHeader {
	//BRM1
	char magic[4];

	//ファイル形式バージョン
	uint32_t version;
	//頂点数
	uint32_t vertexCount;
	//インデックス数
	uint32_t indexCount;
};

//頂点
struct BRVertex {
	//位置座標
	float px;
	float py;
	float pz;

	//法線
	float nx;
	float ny;
	float nz;
	//UV
	float u;
	float v;
};