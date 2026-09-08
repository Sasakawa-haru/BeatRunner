#pragma once

#include<d3d11.h>
#include<cstdint>
#include<string>

#include"Common/BRModelFormat.h"
#include"Transform.h"

class BRModel
{
public:
	BRModel();
	~BRModel();

	bool Load(const std::string& filePath);

	void Draw(Transform& transform);

	void Release();

	bool IsLoaded()const
	{
		return isLoaded_;
	}

	uint32_t GetIndexCount() const 
	{
		return indexCount_;
	}

private:
	struct CONSTANT_BUFFER
	{
		DirectX::XMMATRIX worldVewProj;
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX normalTrans;

		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 speculer;

		float shininess;

		// 16byte境界合わせ
		float padding[3];

		DirectX::XMFLOAT4 cameraPosition;
		DirectX::XMFLOAT4 lightDirection;

		int isTexture;

		float padding2[3];
	};

private:
	ID3D11Buffer* vertexBuffer_ = nullptr;
	ID3D11Buffer* indexBuffer_ = nullptr;

	ID3D11Buffer* constantBuffer_ = nullptr;

	uint32_t vertexCount_ = 0;
	uint32_t indexCount_ = 0;

	bool isLoaded_ = false;
};