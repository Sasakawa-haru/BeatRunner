#pragma once

#include<d3d11.h>
#include<cstdint>
#include<string>

#include"Common/BRModelFormat.h"

class BRModel
{
public:
	BRModel();
	~BRModel();

	bool Load(const std::string& filePath);

	void Draw();

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
	ID3D11Buffer* vertexBuffer_ = nullptr;
	ID3D11Buffer* indexBuffer_ = nullptr;

	uint32_t vertexCount_ = 0;
	uint32_t indexCount_ = 0;

	bool isLoaded_ = false;
};