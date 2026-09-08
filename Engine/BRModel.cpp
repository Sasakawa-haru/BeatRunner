#include "BRModel.h"

#include "Engine/Direct3D.h"

#include <fstream>
#include <vector>
#include <cstring>


BRModel::BRModel()
{
}


BRModel::~BRModel()
{
    Release();
}


bool BRModel::Load(const std::string& filePath)
{
    Release();

    // =========================================
    // BRMファイルを開く
    // =========================================

    std::ifstream file(
        filePath,
        std::ios::binary
    );

    if (!file)
    {
        return false;
    }


    // =========================================
    // Header読み込み
    // =========================================

    BRModelHeader header{};

    file.read(
        reinterpret_cast<char*>(&header),
        sizeof(header)
    );

    if (!file)
    {
        return false;
    }


    // =========================================
    // ファイル形式確認
    // =========================================

    if (std::memcmp(
        header.magic,
        "BRM1",
        4) != 0)
    {
        return false;
    }

    if (header.version != 1)
    {
        return false;
    }

    if (header.vertexCount == 0
        || header.indexCount == 0)
    {
        return false;
    }


    vertexCount_ =
        header.vertexCount;

    indexCount_ =
        header.indexCount;


    // =========================================
    // Vertex読み込み
    // =========================================

    std::vector<BRVertex> vertices(
        vertexCount_
    );

    file.read(
        reinterpret_cast<char*>(
            vertices.data()
            ),
        sizeof(BRVertex)
        * vertices.size()
    );

    if (!file)
    {
        Release();

        return false;
    }


    // =========================================
    // Index読み込み
    // =========================================

    std::vector<uint32_t> indices(
        indexCount_
    );

    file.read(
        reinterpret_cast<char*>(
            indices.data()
            ),
        sizeof(uint32_t)
        * indices.size()
    );

    if (!file)
    {
        Release();

        return false;
    }


    // =========================================
    // Direct3D初期化確認
    // =========================================

    if (!Direct3D::pDevice_)
    {
        Release();

        return false;
    }


    // =========================================
    // VertexBuffer作成
    // =========================================

    D3D11_BUFFER_DESC vertexDesc{};

    vertexDesc.ByteWidth =
        static_cast<UINT>(
            sizeof(BRVertex)
            * vertices.size()
            );

    vertexDesc.Usage =
        D3D11_USAGE_DEFAULT;

    vertexDesc.BindFlags =
        D3D11_BIND_VERTEX_BUFFER;

    vertexDesc.CPUAccessFlags = 0;

    vertexDesc.MiscFlags = 0;

    vertexDesc.StructureByteStride = 0;


    D3D11_SUBRESOURCE_DATA vertexData{};

    vertexData.pSysMem =
        vertices.data();


    HRESULT hr =
        Direct3D::pDevice_->CreateBuffer(
            &vertexDesc,
            &vertexData,
            &vertexBuffer_
        );

    if (FAILED(hr))
    {
        Release();

        return false;
    }


    // =========================================
    // IndexBuffer作成
    // =========================================

    D3D11_BUFFER_DESC indexDesc{};

    indexDesc.ByteWidth =
        static_cast<UINT>(
            sizeof(uint32_t)
            * indices.size()
            );

    indexDesc.Usage =
        D3D11_USAGE_DEFAULT;

    indexDesc.BindFlags =
        D3D11_BIND_INDEX_BUFFER;

    indexDesc.CPUAccessFlags = 0;

    indexDesc.MiscFlags = 0;

    indexDesc.StructureByteStride = 0;


    D3D11_SUBRESOURCE_DATA indexData{};

    indexData.pSysMem =
        indices.data();


    hr =
        Direct3D::pDevice_->CreateBuffer(
            &indexDesc,
            &indexData,
            &indexBuffer_
        );

    if (FAILED(hr))
    {
        Release();

        return false;
    }


    isLoaded_ = true;

    return true;
}


void BRModel::Draw()
{
    if (!isLoaded_)
    {
        return;
    }

    if (!vertexBuffer_
        || !indexBuffer_)
    {
        return;
    }

    if (!Direct3D::pContext_)
    {
        return;
    }


    // =========================================
    // 3D Shader設定
    // =========================================

    Direct3D::SetShader(
        Direct3D::SHADER_3D
    );


    // =========================================
    // VertexBuffer設定
    // =========================================

    UINT stride =
        sizeof(BRVertex);

    UINT offset = 0;

    Direct3D::pContext_->
        IASetVertexBuffers(
            0,
            1,
            &vertexBuffer_,
            &stride,
            &offset
        );


    // =========================================
    // IndexBuffer設定
    // =========================================

    Direct3D::pContext_->
        IASetIndexBuffer(
            indexBuffer_,
            DXGI_FORMAT_R32_UINT,
            0
        );


    // =========================================
    // TriangleList
    // =========================================

    Direct3D::pContext_->
        IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );


    // =========================================
    // 描画
    // =========================================

    Direct3D::pContext_->
        DrawIndexed(
            indexCount_,
            0,
            0
        );
}


void BRModel::Release()
{
    if (indexBuffer_)
    {
        indexBuffer_->Release();

        indexBuffer_ = nullptr;
    }

    if (vertexBuffer_)
    {
        vertexBuffer_->Release();

        vertexBuffer_ = nullptr;
    }

    vertexCount_ = 0;

    indexCount_ = 0;

    isLoaded_ = false;
}