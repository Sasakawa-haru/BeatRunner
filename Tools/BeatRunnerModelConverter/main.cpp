#include <fbxsdk.h>

#include "Common/BRModelFormat.h"

#include <Windows.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


// ============================================================
// UTF-16 → UTF-8
// 日本語パス対応
// ============================================================

std::string WideToUtf8(const std::wstring& text)
{
    if (text.empty())
    {
        return {};
    }

    const int size =
        WideCharToMultiByte(
            CP_UTF8,
            0,
            text.c_str(),
            -1,
            nullptr,
            0,
            nullptr,
            nullptr);

    if (size <= 0)
    {
        return {};
    }

    std::string result(
        static_cast<size_t>(size),
        '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        text.c_str(),
        -1,
        result.data(),
        size,
        nullptr,
        nullptr);

    if (!result.empty()
        && result.back() == '\0')
    {
        result.pop_back();
    }

    return result;
}


// ============================================================
// FBX行列 → float[16]
// ============================================================

void CopyMatrix(
    const FbxAMatrix& src,
    float dst[16])
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            dst[row * 4 + col] =
                static_cast<float>(
                    src.Get(row, col));
        }
    }
}


// ============================================================
// 一時的なControlPoint用BoneWeight
// ============================================================

struct TempWeight
{
    int32_t boneIndex[BRM_MAX_BONE_INFLUENCE];
    float boneWeight[BRM_MAX_BONE_INFLUENCE];

    TempWeight()
    {
        for (uint32_t i = 0;
            i < BRM_MAX_BONE_INFLUENCE;
            ++i)
        {
            boneIndex[i] = -1;
            boneWeight[i] = 0.0f;
        }
    }
};


// ============================================================
// ウェイト追加
// 最大4本まで
// ============================================================

void AddBoneWeight(
    TempWeight& dst,
    int32_t boneIndex,
    float weight)
{
    if (weight <= 0.0f)
    {
        return;
    }

    // 空いている場所
    for (uint32_t i = 0;
        i < BRM_MAX_BONE_INFLUENCE;
        ++i)
    {
        if (dst.boneIndex[i] < 0)
        {
            dst.boneIndex[i] = boneIndex;
            dst.boneWeight[i] = weight;

            return;
        }
    }


    // 4本埋まっている場合は
    // 一番小さいウェイトと比較
    uint32_t smallestIndex = 0;

    for (uint32_t i = 1;
        i < BRM_MAX_BONE_INFLUENCE;
        ++i)
    {
        if (dst.boneWeight[i]
            < dst.boneWeight[smallestIndex])
        {
            smallestIndex = i;
        }
    }

    if (weight
            > dst.boneWeight[smallestIndex])
    {
        dst.boneIndex[smallestIndex] =
            boneIndex;

        dst.boneWeight[smallestIndex] =
            weight;
    }
}


// ============================================================
// ウェイト正規化
// ============================================================

void NormalizeWeight(
    TempWeight& weight)
{
    float total = 0.0f;

    for (uint32_t i = 0;
        i < BRM_MAX_BONE_INFLUENCE;
        ++i)
    {
        total += weight.boneWeight[i];
    }

    if (total <= 0.0f)
    {
        return;
    }

    for (uint32_t i = 0;
        i < BRM_MAX_BONE_INFLUENCE;
        ++i)
    {
        weight.boneWeight[i] /= total;
    }
}


// ============================================================
// Skeletonノード収集
// ============================================================

void CollectSkeletonNodes(
    FbxNode* node,
    std::vector<FbxNode*>& boneNodes)
{
    if (!node)
    {
        return;
    }

    FbxNodeAttribute* attribute =
        node->GetNodeAttribute();

    if (attribute
        && attribute->GetAttributeType()
        == FbxNodeAttribute::eSkeleton)
    {
        boneNodes.push_back(node);
    }


    const int childCount =
        node->GetChildCount();

    for (int i = 0;
        i < childCount;
        ++i)
    {
        CollectSkeletonNodes(
            node->GetChild(i),
            boneNodes);
    }
}


// ============================================================
// Bone情報作成
// ============================================================

void BuildBones(
    const std::vector<FbxNode*>& boneNodes,
    std::vector<BRBone>& bones,
    std::unordered_map<FbxNode*, int32_t>& boneMap)
{
    bones.clear();
    boneMap.clear();

    bones.resize(
        boneNodes.size());


    // --------------------------------------------------------
    // Bone index作成
    // --------------------------------------------------------

    for (size_t i = 0;
        i < boneNodes.size();
        ++i)
    {
        boneMap[boneNodes[i]] =
            static_cast<int32_t>(i);
    }


    // --------------------------------------------------------
    // Bone情報
    // --------------------------------------------------------

    for (size_t i = 0;
        i < boneNodes.size();
        ++i)
    {
        FbxNode* node =
            boneNodes[i];

        BRBone bone{};


        // Bone名
        strncpy_s(
            bone.name,
            BRM_BONE_NAME_LENGTH,
            node->GetName(),
            _TRUNCATE);


        // 親Bone
        bone.parentIndex = -1;

        FbxNode* parent =
            node->GetParent();

        auto parentIt =
            boneMap.find(parent);

        if (parentIt != boneMap.end())
        {
            bone.parentIndex =
                parentIt->second;
        }


        // bindPoseは後でClusterから設定する
        // とりあえずIdentity
        for (int j = 0;
            j < 16;
            ++j)
        {
            bone.bindPose[j] =
                (j % 5 == 0)
                ? 1.0f
                : 0.0f;
        }


        bones[i] = bone;
    }
}


// ============================================================
// ClusterからBindPoseを設定
// ============================================================

void ReadBindPose(
    FbxMesh* mesh,
    std::vector<BRBone>& bones,
    const std::unordered_map<FbxNode*, int32_t>& boneMap)
{
    if (!mesh)
    {
        return;
    }


    const int skinCount =
        mesh->GetDeformerCount(
            FbxDeformer::eSkin);


    for (int skinIndex = 0;
        skinIndex < skinCount;
        ++skinIndex)
    {
        FbxSkin* skin =
            static_cast<FbxSkin*>(
                mesh->GetDeformer(
                    skinIndex,
                    FbxDeformer::eSkin));

        if (!skin)
        {
            continue;
        }


        const int clusterCount =
            skin->GetClusterCount();


        for (int clusterIndex = 0;
            clusterIndex < clusterCount;
            ++clusterIndex)
        {
            FbxCluster* cluster =
                skin->GetCluster(
                    clusterIndex);

            if (!cluster)
            {
                continue;
            }


            FbxNode* linkNode =
                cluster->GetLink();

            if (!linkNode)
            {
                continue;
            }


            auto it =
                boneMap.find(linkNode);

            if (it == boneMap.end())
            {
                continue;
            }


            const int32_t boneIndex =
                it->second;


            // MeshのBind Global
            FbxAMatrix meshBindMatrix;

            cluster->GetTransformMatrix(
                meshBindMatrix);


            // BoneのBind Global
            FbxAMatrix boneBindMatrix;

            cluster->GetTransformLinkMatrix(
                boneBindMatrix);


            // Offset Matrix
            //
            // Vertex(Local)
            // → MeshBind
            // → BoneBind^-1
            const FbxAMatrix offsetMatrix =
                boneBindMatrix.Inverse()
                * meshBindMatrix;


            CopyMatrix(
                offsetMatrix,
                bones[boneIndex].bindPose);
        }
    }
}


// ============================================================
// MeshのControlPointごとのBoneWeight取得
// ============================================================

std::vector<TempWeight> BuildControlPointWeights(
    FbxMesh* mesh,
    const std::unordered_map<FbxNode*, int32_t>& boneMap)
{
    std::vector<TempWeight> weights;

    if (!mesh)
    {
        return weights;
    }


    weights.resize(
        mesh->GetControlPointsCount());


    const int skinCount =
        mesh->GetDeformerCount(
            FbxDeformer::eSkin);


    for (int skinIndex = 0;
        skinIndex < skinCount;
        ++skinIndex)
    {
        FbxSkin* skin =
            static_cast<FbxSkin*>(
                mesh->GetDeformer(
                    skinIndex,
                    FbxDeformer::eSkin));

        if (!skin)
        {
            continue;
        }


        const int clusterCount =
            skin->GetClusterCount();


        for (int clusterIndex = 0;
            clusterIndex < clusterCount;
            ++clusterIndex)
        {
            FbxCluster* cluster =
                skin->GetCluster(
                    clusterIndex);

            if (!cluster)
            {
                continue;
            }


            FbxNode* linkNode =
                cluster->GetLink();

            if (!linkNode)
            {
                continue;
            }


            auto boneIt =
                boneMap.find(linkNode);

            if (boneIt == boneMap.end())
            {
                continue;
            }


            const int32_t boneIndex =
                boneIt->second;


            int* controlPointIndices =
                cluster->GetControlPointIndices();

            double* controlPointWeights =
                cluster->GetControlPointWeights();

            const int count =
                cluster->GetControlPointIndicesCount();


            for (int i = 0;
                i < count;
                ++i)
            {
                const int cpIndex =
                    controlPointIndices[i];

                if (cpIndex < 0
                    || cpIndex
                    >= static_cast<int>(
                        weights.size()))
                {
                    continue;
                }


                AddBoneWeight(
                    weights[cpIndex],
                    boneIndex,
                    static_cast<float>(
                        controlPointWeights[i]));
            }
        }
    }


    // 全ControlPointを正規化
    for (TempWeight& weight : weights)
    {
        NormalizeWeight(weight);
    }


    return weights;
}


// ============================================================
// Mesh変換
// ============================================================

bool ConvertMesh(
    FbxMesh* mesh,
    std::vector<BRVertex>& vertices,
    std::vector<uint32_t>& indices,
    std::vector<BRBone>& bones,
    const std::unordered_map<FbxNode*, int32_t>& boneMap)
{
    if (!mesh)
    {
        return false;
    }


    // BindPose
    ReadBindPose(
        mesh,
        bones,
        boneMap);


    // ControlPoint BoneWeight
    const std::vector<TempWeight> weights =
        BuildControlPointWeights(
            mesh,
            boneMap);


    // --------------------------------------------------------
    // UV
    // --------------------------------------------------------

    FbxStringList uvSetNames;

    mesh->GetUVSetNames(
        uvSetNames);


    const char* uvSetName =
        nullptr;

    if (uvSetNames.GetCount() > 0)
    {
        uvSetName =
            uvSetNames.GetStringAt(0);
    }


    // --------------------------------------------------------
    // Polygon
    // --------------------------------------------------------

    const int polygonCount =
        mesh->GetPolygonCount();


    for (int polygon = 0;
        polygon < polygonCount;
        ++polygon)
    {
        const int polygonSize =
            mesh->GetPolygonSize(
                polygon);

        if (polygonSize != 3)
        {
            std::cerr
                << "TriangleではないPolygonがあります\n";

            continue;
        }


        for (int polygonVertex = 0;
            polygonVertex < 3;
            ++polygonVertex)
        {
            BRVertex vertex{};


            // Bone初期化
            for (uint32_t i = 0;
                i < BRM_MAX_BONE_INFLUENCE;
                ++i)
            {
                vertex.boneIndex[i] = -1;
                vertex.boneWeight[i] = 0.0f;
            }


            // ================================================
            // Position
            // ================================================

            const int controlPointIndex =
                mesh->GetPolygonVertex(
                    polygon,
                    polygonVertex);


            const FbxVector4 position =
                mesh->GetControlPointAt(
                    controlPointIndex);


            vertex.px =
                static_cast<float>(
                    position[0]);

            vertex.py =
                static_cast<float>(
                    position[1]);

            vertex.pz =
                static_cast<float>(
                    position[2]);


            // ================================================
            // Normal
            // ================================================

            FbxVector4 normal;

            if (mesh->GetPolygonVertexNormal(
                polygon,
                polygonVertex,
                normal))
            {
                normal.Normalize();

                vertex.nx =
                    static_cast<float>(
                        normal[0]);

                vertex.ny =
                    static_cast<float>(
                        normal[1]);

                vertex.nz =
                    static_cast<float>(
                        normal[2]);
            }


            // ================================================
            // UV
            // ================================================

            if (uvSetName)
            {
                FbxVector2 uv;

                bool unmapped = false;


                if (mesh->GetPolygonVertexUV(
                    polygon,
                    polygonVertex,
                    uvSetName,
                    uv,
                    unmapped))
                {
                    vertex.u =
                        static_cast<float>(
                            uv[0]);

                    vertex.v =
                        1.0f
                        - static_cast<float>(
                            uv[1]);
                }
            }


            // ================================================
            // Bone
            // ================================================

            if (controlPointIndex >= 0
                && controlPointIndex
                < static_cast<int>(
                    weights.size()))
            {
                for (uint32_t i = 0;
                    i < BRM_MAX_BONE_INFLUENCE;
                    ++i)
                {
                    vertex.boneIndex[i] =
                        weights[
                            controlPointIndex
                        ].boneIndex[i];

                    vertex.boneWeight[i] =
                        weights[
                            controlPointIndex
                        ].boneWeight[i];
                }
            }


            // ================================================
            // Vertex / Index
            // ================================================

            const uint32_t index =
                static_cast<uint32_t>(
                    vertices.size());


            vertices.push_back(
                vertex);

            indices.push_back(
                index);
        }
    }


    return true;
}


// ============================================================
// Scene Node読み込み
// ============================================================

void ReadNode(
    FbxNode* node,
    std::vector<BRVertex>& vertices,
    std::vector<uint32_t>& indices,
    std::vector<BRBone>& bones,
    const std::unordered_map<FbxNode*, int32_t>& boneMap)
{
    if (!node)
    {
        return;
    }


    FbxNodeAttribute* attribute =
        node->GetNodeAttribute();


    if (attribute
        && attribute->GetAttributeType()
        == FbxNodeAttribute::eMesh)
    {
        ConvertMesh(
            node->GetMesh(),
            vertices,
            indices,
            bones,
            boneMap);
    }


    const int childCount =
        node->GetChildCount();


    for (int i = 0;
        i < childCount;
        ++i)
    {
        ReadNode(
            node->GetChild(i),
            vertices,
            indices,
            bones,
            boneMap);
    }
}


// ============================================================
// Animation取得
// ============================================================

void BuildAnimation(
    FbxScene* scene,
    const std::vector<FbxNode*>& boneNodes,
    std::vector<BRBoneTransform>& animation,
    uint32_t& frameCount)
{
    frameCount = 0;
    animation.clear();


    if (!scene
        || boneNodes.empty())
    {
        return;
    }


    // SceneのTimeMode
    const FbxTime::EMode timeMode =
        scene->GetGlobalSettings()
        .GetTimeMode();


    // Animationの時間範囲
    FbxTimeSpan timeSpan;

    scene->GetGlobalSettings()
        .GetTimelineDefaultTimeSpan(
            timeSpan);


    const FbxTime startTime =
        timeSpan.GetStart();

    const FbxTime stopTime =
        timeSpan.GetStop();


    const FbxLongLong startFrame =
        startTime.GetFrameCount(
            timeMode);

    const FbxLongLong stopFrame =
        stopTime.GetFrameCount(
            timeMode);


    if (stopFrame < startFrame)
    {
        return;
    }


    frameCount =
        static_cast<uint32_t>(
            stopFrame
            - startFrame
            + 1);


    animation.reserve(
        static_cast<size_t>(
            frameCount)
        * boneNodes.size());


    // --------------------------------------------------------
    // 各Frame
    // --------------------------------------------------------

    for (FbxLongLong frame =
        startFrame;
        frame <= stopFrame;
        ++frame)
    {
        FbxTime time;

        time.SetFrame(
            frame,
            timeMode);


        // ----------------------------------------------------
        // 各Bone
        // ----------------------------------------------------

        for (FbxNode* boneNode :
            boneNodes)
        {
            BRBoneTransform transform{};


            const FbxAMatrix globalMatrix =
                boneNode
                ->EvaluateGlobalTransform(
                    time);


            CopyMatrix(
                globalMatrix,
                transform.matrix);


            animation.push_back(
                transform);
        }
    }
}


// ============================================================
// BRM保存
// ============================================================

bool SaveBRModel(
    const std::filesystem::path& fileName,
    const std::vector<BRVertex>& vertices,
    const std::vector<uint32_t>& indices,
    const std::vector<BRBone>& bones,
    const std::vector<BRBoneTransform>& animation,
    uint32_t frameCount)
{
    // --------------------------------------------------------
    // 出力フォルダ作成
    // --------------------------------------------------------

    const std::filesystem::path parent =
        fileName.parent_path();


    if (!parent.empty())
    {
        std::error_code error;

        std::filesystem::create_directories(
            parent,
            error);

        if (error)
        {
            std::wcerr
                << L"出力フォルダ作成失敗\n";

            return false;
        }
    }


    std::ofstream file(
        fileName,
        std::ios::binary);


    if (!file)
    {
        std::wcerr
            << L"出力ファイルを作成できません\n";

        return false;
    }


    // ========================================================
    // Header
    // ========================================================

    BRModelHeader header{};


    header.magic[0] = 'B';
    header.magic[1] = 'R';
    header.magic[2] = 'M';
    header.magic[3] = '1';

    header.version =
        BRM_VERSION;

    header.vertexCount =
        static_cast<uint32_t>(
            vertices.size());

    header.indexCount =
        static_cast<uint32_t>(
            indices.size());

    header.boneCount =
        static_cast<uint32_t>(
            bones.size());

    header.frameCount =
        frameCount;


    file.write(
        reinterpret_cast<const char*>(
            &header),
        sizeof(header));


    // ========================================================
    // Vertex
    // ========================================================

    if (!vertices.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                vertices.data()),
            sizeof(BRVertex)
            * vertices.size());
    }


    // ========================================================
    // Index
    // ========================================================

    if (!indices.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                indices.data()),
            sizeof(uint32_t)
            * indices.size());
    }


    // ========================================================
    // Bone
    // ========================================================

    if (!bones.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                bones.data()),
            sizeof(BRBone)
            * bones.size());
    }


    // ========================================================
    // Animation
    // ========================================================

    if (!animation.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                animation.data()),
            sizeof(BRBoneTransform)
            * animation.size());
    }


    if (!file)
    {
        std::cerr
            << "BRM書き込み失敗\n";

        return false;
    }


    return true;
}


// ============================================================
// Main
// ============================================================

int wmain(
    int argc,
    wchar_t* argv[])
{
    SetConsoleOutputCP(
        CP_UTF8);


    if (argc < 3)
    {
        std::wcout
            << L"Usage:\n"
            << L"BeatRunnerModelConverter.exe "
            << L"input.fbx output.brm\n";

        return 0;
    }


    const std::wstring inputPathW =
        argv[1];

    const std::wstring outputPathW =
        argv[2];


    const std::filesystem::path inputPath(
        inputPathW);

    const std::filesystem::path outputPath(
        outputPathW);


    // ========================================================
    // ファイル存在確認
    // ========================================================

    if (!std::filesystem::exists(
        inputPath))
    {
        std::wcerr
            << L"入力FBXが存在しません\n"
            << inputPathW
            << L"\n";

        return 1;
    }


    const std::string inputPathUtf8 =
        WideToUtf8(
            inputPathW);


    if (inputPathUtf8.empty())
    {
        std::cerr
            << "パスUTF-8変換失敗\n";

        return 1;
    }


    // ========================================================
    // FBX Manager
    // ========================================================

    FbxManager* manager =
        FbxManager::Create();


    if (!manager)
    {
        std::cerr
            << "FbxManager作成失敗\n";

        return 1;
    }


    FbxIOSettings* ioSettings =
        FbxIOSettings::Create(
            manager,
            IOSROOT);


    manager->SetIOSettings(
        ioSettings);


    // ========================================================
    // Scene
    // ========================================================

    FbxScene* scene =
        FbxScene::Create(
            manager,
            "Scene");


    // ========================================================
    // Importer
    // ========================================================

    FbxImporter* importer =
        FbxImporter::Create(
            manager,
            "Importer");


    if (!importer->Initialize(
        inputPathUtf8.c_str(),
        -1,
        manager->GetIOSettings()))
    {
        std::cerr
            << "FBX読み込み失敗\n";

        std::cerr
            << importer
            ->GetStatus()
            .GetErrorString()
            << "\n";


        importer->Destroy();
        manager->Destroy();

        return 1;
    }


    if (!importer->Import(
        scene))
    {
        std::cerr
            << "FBX Import失敗\n";

        std::cerr
            << importer
            ->GetStatus()
            .GetErrorString()
            << "\n";


        importer->Destroy();
        manager->Destroy();

        return 1;
    }


    importer->Destroy();


    // ========================================================
    // Triangle化
    // ========================================================

    FbxGeometryConverter converter(
        manager);


    if (!converter.Triangulate(
        scene,
        true))
    {
        std::cerr
            << "三角形化失敗\n";

        manager->Destroy();

        return 1;
    }


    // ========================================================
    // Skeleton
    // ========================================================

    std::vector<FbxNode*> boneNodes;


    FbxNode* root =
        scene->GetRootNode();


    if (root)
    {
        CollectSkeletonNodes(
            root,
            boneNodes);
    }


    std::vector<BRBone> bones;

    std::unordered_map<
        FbxNode*,
        int32_t> boneMap;


    BuildBones(
        boneNodes,
        bones,
        boneMap);


    // ========================================================
    // Mesh
    // ========================================================

    std::vector<BRVertex> vertices;

    std::vector<uint32_t> indices;


    if (root)
    {
        ReadNode(
            root,
            vertices,
            indices,
            bones,
            boneMap);
    }


    // ========================================================
    // Animation
    // ========================================================

    std::vector<BRBoneTransform>
        animation;


    uint32_t frameCount = 0;


    BuildAnimation(
        scene,
        boneNodes,
        animation,
        frameCount);


    // ========================================================
    // 確認表示
    // ========================================================

    std::cout
        << "Vertex : "
        << vertices.size()
        << "\n";

    std::cout
        << "Index  : "
        << indices.size()
        << "\n";

    std::cout
        << "Bone   : "
        << bones.size()
        << "\n";

    std::cout
        << "Frame  : "
        << frameCount
        << "\n";


    if (vertices.empty())
    {
        std::cerr
            << "Meshがありません\n";

        manager->Destroy();

        return 1;
    }


    // ========================================================
    // BRM保存
    // ========================================================

    if (!SaveBRModel(
        outputPath,
        vertices,
        indices,
        bones,
        animation,
        frameCount))
    {
        manager->Destroy();

        return 1;
    }


    std::wcout
        << L"Convert成功\n";

    std::wcout
        << inputPathW
        << L"\n -> "
        << outputPathW
        << L"\n";


    manager->Destroy();

    return 0;
}