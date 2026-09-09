#include "BRModel.h"

#include "Direct3D.h"
#include "Camera.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>


using namespace DirectX;


// ============================================================
// Constructor
// ============================================================

BRModel::BRModel()
{
}


// ============================================================
// Destructor
// ============================================================

BRModel::~BRModel()
{
    Release();
}


// ============================================================
// Load
// ============================================================

bool BRModel::Load(
    const std::string& filePath)
{
    Release();


    // =========================================
    // ファイルを開く
    // =========================================

    std::ifstream file(
        std::filesystem::path(filePath),
        std::ios::binary
    );


    if (!file)
    {
        return false;
    }


    // =========================================
    // Header
    // =========================================

    BRModelHeader header{};


    file.read(
        reinterpret_cast<char*>(
            &header
            ),
        sizeof(header)
    );


    if (!file)
    {
        return false;
    }


    // =========================================
    // Magic確認
    // =========================================

    if (std::memcmp(
        header.magic,
        "BRM1",
        4) != 0)
    {
        return false;
    }


    // =========================================
    // Version確認
    // =========================================

    if (header.version != BRM_VERSION)
    {
        return false;
    }


    // =========================================
    // データ数確認
    // =========================================

    if (header.vertexCount == 0
        || header.indexCount == 0)
    {
        return false;
    }


    vertexCount_ =
        header.vertexCount;


    indexCount_ =
        header.indexCount;


    frameCount_ =
        header.frameCount;


    // =========================================
    // Vertex
    // =========================================

    originalVertices_.resize(
        vertexCount_
    );


    file.read(
        reinterpret_cast<char*>(
            originalVertices_.data()
            ),
        sizeof(BRVertex)
        * originalVertices_.size()
    );


    if (!file)
    {
        Release();

        return false;
    }


    skinnedVertices_ =
        originalVertices_;


    // =========================================
    // Index
    // =========================================

    std::vector<uint32_t> indices;


    indices.resize(
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
    // Bone
    // =========================================

    bones_.resize(
        header.boneCount
    );


    if (!bones_.empty())
    {
        file.read(
            reinterpret_cast<char*>(
                bones_.data()
                ),
            sizeof(BRBone)
            * bones_.size()
        );


        if (!file)
        {
            Release();

            return false;
        }
    }


    // =========================================
    // Animation
    // =========================================

    const size_t animationCount =
        static_cast<size_t>(
            header.frameCount
            )
        *
        static_cast<size_t>(
            header.boneCount
            );


    animation_.resize(
        animationCount
    );


    if (!animation_.empty())
    {
        file.read(
            reinterpret_cast<char*>(
                animation_.data()
                ),
            sizeof(BRBoneTransform)
            * animation_.size()
        );


        if (!file)
        {
            Release();

            return false;
        }
    }


    // =========================================
    // Direct3D Device確認
    // =========================================

    if (!Direct3D::pDevice_)
    {
        Release();

        return false;
    }


    // =========================================
    // VertexBuffer
    //
    // CPUスキニングで毎Frame書き換えるため
    // DYNAMIC
    // =========================================

    D3D11_BUFFER_DESC vertexDesc{};


    vertexDesc.ByteWidth =
        static_cast<UINT>(
            sizeof(BRVertex)
            * originalVertices_.size()
            );


    vertexDesc.Usage =
        D3D11_USAGE_DYNAMIC;


    vertexDesc.BindFlags =
        D3D11_BIND_VERTEX_BUFFER;


    vertexDesc.CPUAccessFlags =
        D3D11_CPU_ACCESS_WRITE;


    vertexDesc.MiscFlags =
        0;


    vertexDesc.StructureByteStride =
        0;


    D3D11_SUBRESOURCE_DATA vertexData{};


    vertexData.pSysMem =
        originalVertices_.data();


    HRESULT hr =
        Direct3D::pDevice_
        ->CreateBuffer(
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
    // IndexBuffer
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


    indexDesc.CPUAccessFlags =
        0;


    indexDesc.MiscFlags =
        0;


    indexDesc.StructureByteStride =
        0;


    D3D11_SUBRESOURCE_DATA indexData{};


    indexData.pSysMem =
        indices.data();


    hr =
        Direct3D::pDevice_
        ->CreateBuffer(
            &indexDesc,
            &indexData,
            &indexBuffer_
        );


    if (FAILED(hr))
    {
        Release();

        return false;
    }


    // =========================================
    // ConstantBuffer
    //
    // FbxParts::IntConstantBufferと同じ方式
    // =========================================

    D3D11_BUFFER_DESC cbDesc{};


    cbDesc.ByteWidth =
        sizeof(CONSTANT_BUFFER);


    cbDesc.Usage =
        D3D11_USAGE_DYNAMIC;


    cbDesc.BindFlags =
        D3D11_BIND_CONSTANT_BUFFER;


    cbDesc.CPUAccessFlags =
        D3D11_CPU_ACCESS_WRITE;


    cbDesc.MiscFlags =
        0;


    cbDesc.StructureByteStride =
        0;


    hr =
        Direct3D::pDevice_
        ->CreateBuffer(
            &cbDesc,
            nullptr,
            &constantBuffer_
        );


    if (FAILED(hr))
    {
        Release();

        return false;
    }


    isLoaded_ = true;


    return true;
}


// ============================================================
// CPU Skinning
// ============================================================

void BRModel::UpdateSkinning(
    int frame)
{
    if (!isLoaded_)
    {
        return;
    }


    if (originalVertices_.empty())
    {
        return;
    }


    if (!Direct3D::pContext_)
    {
        return;
    }


    // =========================================
    // Animation無し
    // =========================================

    if (bones_.empty()
        || animation_.empty()
        || frameCount_ == 0)
    {
        skinnedVertices_ =
            originalVertices_;
    }
    else
    {
        // =====================================
        // Frame補正
        // =====================================

        if (frame < 0)
        {
            frame = 0;
        }


        if (frame >=
            static_cast<int>(
                frameCount_
                ))
        {
            frame =
                static_cast<int>(
                    frameCount_ - 1
                    );
        }


        const size_t boneCount =
            bones_.size();


        // =====================================
        // 全頂点
        // =====================================

        for (size_t vertexIndex = 0;
            vertexIndex
            < originalVertices_.size();
            ++vertexIndex)
        {
            const BRVertex& src =
                originalVertices_[
                    vertexIndex
                ];


            BRVertex dst =
                src;


            XMVECTOR sourcePosition =
                XMVectorSet(
                    src.px,
                    src.py,
                    src.pz,
                    1.0f
                );


            XMVECTOR sourceNormal =
                XMVectorSet(
                    src.nx,
                    src.ny,
                    src.nz,
                    0.0f
                );


            XMVECTOR resultPosition =
                XMVectorZero();


            XMVECTOR resultNormal =
                XMVectorZero();


            float totalWeight =
                0.0f;


            // =================================
            // 最大4Bone
            // =================================

            for (uint32_t influence = 0;
                influence
                < BRM_MAX_BONE_INFLUENCE;
                ++influence)
            {
                const int32_t boneIndex =
                    src.boneIndex[
                        influence
                    ];


                const float weight =
                    src.boneWeight[
                        influence
                    ];


                if (boneIndex < 0
                    || weight <= 0.0f)
                {
                    continue;
                }


                if (boneIndex >=
                    static_cast<int32_t>(
                        boneCount
                        ))
                {
                    continue;
                }


                // =============================
                // BindPose
                // =============================

                XMFLOAT4X4 bindPose{};


                for (int row = 0;
                    row < 4;
                    ++row)
                {
                    for (int col = 0;
                        col < 4;
                        ++col)
                    {
                        bindPose.m[row][col] =
                            bones_[
                                boneIndex
                            ]
                            .bindPose[
                                row * 4 + col
                            ];
                    }
                }


                const XMMATRIX bindMatrix =
                    XMLoadFloat4x4(
                        &bindPose
                    );


                // =============================
                // Animation Matrix
                // =============================

                const size_t animationIndex =
                    static_cast<size_t>(
                        frame
                        )
                    *
                    boneCount
                    +
                    static_cast<size_t>(
                        boneIndex
                        );


                if (animationIndex
                    >= animation_.size())
                {
                    continue;
                }


                XMFLOAT4X4 animationPose{};


                for (int row = 0;
                    row < 4;
                    ++row)
                {
                    for (int col = 0;
                        col < 4;
                        ++col)
                    {
                        animationPose.m[row][col] =
                            animation_[
                                animationIndex
                            ]
                            .matrix[
                                row * 4 + col
                            ];
                    }
                }


                const XMMATRIX currentMatrix =
                    XMLoadFloat4x4(
                        &animationPose
                    );


                // =============================
                // Skin Matrix
                // =============================

                const XMMATRIX skinMatrix =
                    XMMatrixInverse(
                        nullptr,
                        bindMatrix
                    )
                    *
                    currentMatrix;


                // =============================
                // Position
                // =============================

                resultPosition +=
                    XMVector3TransformCoord(
                        sourcePosition,
                        skinMatrix
                    )
                    *
                    weight;


                // =============================
                // Normal
                // =============================

                resultNormal +=
                    XMVector3TransformNormal(
                        sourceNormal,
                        skinMatrix
                    )
                    *
                    weight;


                totalWeight +=
                    weight;
            }


            // =================================
            // Bone無し
            // =================================

            if (totalWeight <=
                0.000001f)
            {
                dst.px =
                    src.px;

                dst.py =
                    src.py;

                dst.pz =
                    src.pz;


                dst.nx =
                    src.nx;

                dst.ny =
                    src.ny;

                dst.nz =
                    src.nz;
            }
            else
            {
                resultPosition /=
                    totalWeight;


                resultNormal /=
                    totalWeight;


                resultNormal =
                    XMVector3Normalize(
                        resultNormal
                    );


                XMFLOAT3 position{};

                XMFLOAT3 normal{};


                XMStoreFloat3(
                    &position,
                    resultPosition
                );


                XMStoreFloat3(
                    &normal,
                    resultNormal
                );


                dst.px =
                    position.x;

                dst.py =
                    position.y;

                dst.pz =
                    position.z;


                dst.nx =
                    normal.x;

                dst.ny =
                    normal.y;

                dst.nz =
                    normal.z;
            }


            skinnedVertices_[
                vertexIndex
            ] =
                dst;
        }
    }


    // =========================================
    // VertexBufferへ反映
    // =========================================

    D3D11_MAPPED_SUBRESOURCE mapped{};


    HRESULT hr =
        Direct3D::pContext_
        ->Map(
            vertexBuffer_,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped
        );


    if (FAILED(hr))
    {
        return;
    }


    std::memcpy(
        mapped.pData,
        skinnedVertices_.data(),
        sizeof(BRVertex)
        * skinnedVertices_.size()
    );


    Direct3D::pContext_
        ->Unmap(
            vertexBuffer_,
            0
        );
}


// ============================================================
// Draw
// ============================================================

void BRModel::Draw(
    Transform& transform,
    int frame)
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
    // CPU Skinning
    // =========================================

    UpdateSkinning(
        frame
    );


    // =========================================
    // Shader
    // =========================================

    Direct3D::SetShader(
        Direct3D::SHADER_3D
    );


    // =========================================
    // VertexBuffer
    // =========================================

    UINT stride =
        sizeof(BRVertex);


    UINT offset =
        0;


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


    // =========================================
    // Primitive
    // =========================================

    Direct3D::pContext_
        ->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );


    // =========================================
    // ConstantBufferをShaderへ設定
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
    // ConstantBufferデータ
    // =========================================

    CONSTANT_BUFFER cb{};


    const XMMATRIX world =
        transform.GetWorldMatrix();


    cb.worldVewProj =
        XMMatrixTranspose(
            world
            *
            Camera::GetViewMatrix()
            *
            Camera::GetProjectionMatrix()
        );


    cb.world =
        XMMatrixTranspose(
            world
        );


    cb.normalTrans =
        XMMatrixTranspose(
            transform.matRotate_
            *
            XMMatrixInverse(
                nullptr,
                transform.matScale_
            )
        );


    // =========================================
    // Material
    //
    // BRM側にMaterialがまだ無いので仮設定
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


    cb.shininess =
        0.0f;


    // =========================================
    // Camera
    // =========================================

    const XMFLOAT3 cameraPosition =
        Camera::GetPosition();


    cb.cameraPosition =
        XMFLOAT4(
            cameraPosition.x,
            cameraPosition.y,
            cameraPosition.z,
            0.0f
        );


    // =========================================
    // Light
    // =========================================

    cb.lightDirection =
        XMFLOAT4(
            1.0f,
            -1.0f,
            1.0f,
            0.0f
        );


    // Textureはまだ未対応
    cb.isTexture =
        FALSE;


    // =========================================
    // GPUへConstantBufferを書き込む
    // =========================================

    D3D11_MAPPED_SUBRESOURCE mapped{};


    HRESULT hr =
        Direct3D::pContext_
        ->Map(
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


    std::memcpy(
        mapped.pData,
        &cb,
        sizeof(cb)
    );


    Direct3D::pContext_
        ->Unmap(
            constantBuffer_,
            0
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


// ============================================================
// Release
// ============================================================

void BRModel::Release()
{
    if (constantBuffer_)
    {
        constantBuffer_->Release();

        constantBuffer_ =
            nullptr;
    }


    if (indexBuffer_)
    {
        indexBuffer_->Release();

        indexBuffer_ =
            nullptr;
    }


    if (vertexBuffer_)
    {
        vertexBuffer_->Release();

        vertexBuffer_ =
            nullptr;
    }


    originalVertices_.clear();

    skinnedVertices_.clear();

    bones_.clear();

    animation_.clear();


    vertexCount_ =
        0;

    indexCount_ =
        0;

    frameCount_ =
        0;


    isLoaded_ =
        false;
}