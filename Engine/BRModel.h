#pragma once

#include <d3d11.h>
#include <cstdint>
#include <string>
#include <vector>

#include <DirectXMath.h>

#include "Common/BRModelFormat.h"
#include "Transform.h"


class BRModel
{
public:
    BRModel();
    ~BRModel();

    // BRM読み込み
    bool Load(const std::string& filePath);

    // 描画
    void Draw(
        Transform& transform,
        int frame
    );

    // 解放
    void Release();

    bool IsLoaded() const
    {
        return isLoaded_;
    }

    uint32_t GetBoneCount() const
    {
        return static_cast<uint32_t>(
            bones_.size()
            );
    }

    uint32_t GetFrameCount() const
    {
        return frameCount_;
    }


private:
    // CPUスキニング
    void UpdateSkinning(int frame);


private:

    // =========================================
    // Simple3D.hlsl用 ConstantBuffer
    //
    // FbxParts::CONSTANT_BUFFERと同じ並び
    // =========================================

    struct CONSTANT_BUFFER
    {
        DirectX::XMMATRIX worldVewProj;

        DirectX::XMMATRIX normalTrans;

        DirectX::XMMATRIX world;

        DirectX::XMFLOAT4 lightDirection;

        DirectX::XMFLOAT4 diffuse;

        DirectX::XMFLOAT4 ambient;

        DirectX::XMFLOAT4 speculer;

        DirectX::XMFLOAT4 cameraPosition;

        FLOAT shininess;

        BOOL isTexture;
    };


private:

    ID3D11Buffer* constantBuffer_ = nullptr;

    ID3D11Buffer* vertexBuffer_ = nullptr;

    ID3D11Buffer* indexBuffer_ = nullptr;


    uint32_t vertexCount_ = 0;

    uint32_t indexCount_ = 0;

    uint32_t frameCount_ = 0;


    // 元頂点
    std::vector<BRVertex>
        originalVertices_;


    // CPUスキニング後の頂点
    std::vector<BRVertex>
        skinnedVertices_;


    // Bone
    std::vector<BRBone>
        bones_;


    // frameCount × boneCount
    std::vector<BRBoneTransform>
        animation_;


    bool isLoaded_ = false;
};