#include "BRModel.h"

#include "Engine/Direct3D.h"
#include "Engine/Camera.h"

#include <fstream>
#include <vector>
#include <cstring>

using namespace DirectX;

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


void BRModel::Draw(
    Transform& transform)
{
    if (!isLoaded_)
    {
        return;
    }

    if (!vertexBuffer_
        || !indexBuffer_
        || !constantBuffer_)
    {
        return;
    }

    if (!Direct3D::pContext_)
    {
        return;
    }


    // =========================================
    // 3D Shader
    // =========================================

    Direct3D::SetShader(
        Direct3D::SHADER_3D
    );


    // =========================================
    // VertexBuffer
    // =========================================

    UINT stride =
        sizeof(BRVertex);

    UINT offset = 0;

    Direct3D::pContext_
        ->IASetVertexBuffers(
            0,
            1,
            &vertexBuffer_,
            &stride,
            &offset
        );


    // =========================================
    // IndexBuffer
    // =========================================

    Direct3D::pContext_
        ->IASetIndexBuffer(
            indexBuffer_,
            DXGI_FORMAT_R32_UINT,
            0
        );


    Direct3D::pContext_
        ->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );


    // =========================================
    // ConstantBuffer更新
    // =========================================

    CONSTANT_BUFFER cb{};


    const XMMATRIX world =
        transform.GetWorldMatrix();

    const XMMATRIX view =
        Camera::GetViewMatrix();

    const XMMATRIX projection =
        Camera::GetProjectionMatrix();


    cb.worldVewProj =
        XMMatrixTranspose(
            world
            * view
            * projection
        );


    cb.world =
        XMMatrixTranspose(
            world
        );


    cb.normalTrans =
        XMMatrixTranspose(
            transform.matRotate_
            * XMMatrixInverse(
                nullptr,
                transform.matScale_
            )
        );


    // =========================================
    // 仮マテリアル
    // =========================================

    cb.ambient =
        XMFLOAT4(
            0.3f,
            0.3f,
            0.3f,
            1.0f
        );

    cb.diffuse =
        XMFLOAT4(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

    cb.speculer =
        XMFLOAT4(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

    cb.shininess = 0.0f;


    const XMFLOAT3 cameraPos =
        Camera::GetPosition();


    cb.cameraPosition =
        XMFLOAT4(
            cameraPos.x,
            cameraPos.y,
            cameraPos.z,
            0.0f
        );


    cb.lightDirection =
        XMFLOAT4(
            1.0f,
            -1.0f,
            1.0f,
            0.0f
        );


    // まだBRMにTexture情報が無い
    cb.isTexture = 0;


    // =========================================
    // GPUへ送る
    // =========================================

    D3D11_MAPPED_SUBRESOURCE mapped{};


    HRESULT hr =
        Direct3D::pContext_->Map(
            constantBuffer_,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped
        );


    if (FAILED(hr))
    {
        return;
    }


    memcpy(
        mapped.pData,
        &cb,
        sizeof(cb)
    );


    Direct3D::pContext_->Unmap(
        constantBuffer_,
        0
    );


    // =========================================
    // ShaderへConstantBuffer設定
    // =========================================

    Direct3D::pContext_
        ->VSSetConstantBuffers(
            0,
            1,
            &constantBuffer_
        );


    Direct3D::pContext_
        ->PSSetConstantBuffers(
            0,
            1,
            &constantBuffer_
        );


    // =========================================
    // Draw
    // =========================================

    Direct3D::pContext_
        ->DrawIndexed(
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

    if (constantBuffer_) {
        constantBuffer_->Release();
        constantBuffer_ = nullptr;
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