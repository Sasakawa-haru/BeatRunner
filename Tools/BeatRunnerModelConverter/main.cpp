#include <fbxsdk.h>

#include "BRModelFormat.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>


// ============================================
// FBXメッシュを読み込む
// ============================================

bool ConvertMesh(
    FbxMesh* mesh,
    std::vector<BRVertex>& vertices,
    std::vector<uint32_t>& indices)
{
    if (!mesh)
    {
        return false;
    }

    const int polygonCount =
        mesh->GetPolygonCount();

    // ----------------------------------------
    // UVセット名取得
    // ----------------------------------------

    FbxStringList uvSetNames;

    mesh->GetUVSetNames(uvSetNames);

    const char* uvSetName = nullptr;

    if (uvSetNames.GetCount() > 0)
    {
        uvSetName =
            uvSetNames.GetStringAt(0);
    }


    // ----------------------------------------
    // ポリゴンを読む
    // ----------------------------------------

    for (int polygon = 0;
        polygon < polygonCount;
        ++polygon)
    {
        const int polygonSize =
            mesh->GetPolygonSize(polygon);

        // 三角形化済みを前提
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

            // =================================
            // Position
            // =================================

            const int controlPointIndex =
                mesh->GetPolygonVertex(
                    polygon,
                    polygonVertex);

            const FbxVector4 position =
                mesh->GetControlPointAt(
                    controlPointIndex);

            vertex.px =
                static_cast<float>(position[0]);

            vertex.py =
                static_cast<float>(position[1]);

            vertex.pz =
                static_cast<float>(position[2]);


            // =================================
            // Normal
            // =================================

            FbxVector4 normal;

            if (mesh->GetPolygonVertexNormal(
                polygon,
                polygonVertex,
                normal))
            {
                normal.Normalize();

                vertex.nx =
                    static_cast<float>(normal[0]);

                vertex.ny =
                    static_cast<float>(normal[1]);

                vertex.nz =
                    static_cast<float>(normal[2]);
            }


            // =================================
            // UV
            // =================================

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
                        static_cast<float>(uv[0]);

                    // DirectX用にV反転
                    vertex.v =
                        1.0f -
                        static_cast<float>(uv[1]);
                }
            }


            // =================================
            // 頂点追加
            // =================================

            const uint32_t index =
                static_cast<uint32_t>(
                    vertices.size());

            vertices.push_back(vertex);

            indices.push_back(index);
        }
    }

    return true;
}


// ============================================
// ノードを再帰的に検索
// ============================================

void ReadNode(
    FbxNode* node,
    std::vector<BRVertex>& vertices,
    std::vector<uint32_t>& indices)
{
    if (!node)
    {
        return;
    }

    FbxNodeAttribute* attribute =
        node->GetNodeAttribute();

    if (attribute)
    {
        if (attribute->GetAttributeType()
            == FbxNodeAttribute::eMesh)
        {
            FbxMesh* mesh =
                node->GetMesh();

            ConvertMesh(
                mesh,
                vertices,
                indices);
        }
    }


    // 子ノード
    const int childCount =
        node->GetChildCount();

    for (int i = 0;
        i < childCount;
        ++i)
    {
        ReadNode(
            node->GetChild(i),
            vertices,
            indices);
    }
}


// ============================================
// BRM保存
// ============================================

bool SaveBRModel(
    const std::string& fileName,
    const std::vector<BRVertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    std::ofstream file(
        fileName,
        std::ios::binary);

    if (!file)
    {
        std::cerr
            << "出力ファイルを作成できません\n";

        return false;
    }


    BRModelHeader header{};

    header.magic[0] = 'B';
    header.magic[1] = 'R';
    header.magic[2] = 'M';
    header.magic[3] = '1';

    header.version = 1;

    header.vertexCount =
        static_cast<uint32_t>(
            vertices.size());

    header.indexCount =
        static_cast<uint32_t>(
            indices.size());


    // Header
    file.write(
        reinterpret_cast<const char*>(&header),
        sizeof(header));


    // Vertex
    if (!vertices.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                vertices.data()),
            sizeof(BRVertex)
            * vertices.size());
    }


    // Index
    if (!indices.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                indices.data()),
            sizeof(uint32_t)
            * indices.size());
    }


    return true;
}


// ============================================
// main
// ============================================

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout
            << "Usage:\n"
            << "BeatRunnerModelConverter.exe input.fbx output.brm\n";

        return 0;
    }


    const std::string inputPath =
        argv[1];

    const std::string outputPath =
        argv[2];


    // ========================================
    // FBX Manager
    // ========================================

    FbxManager* manager =
        FbxManager::Create();

    if (!manager)
    {
        std::cerr
            << "FbxManager作成失敗\n";

        return 1;
    }


    // IOSettings
    FbxIOSettings* ioSettings =
        FbxIOSettings::Create(
            manager,
            IOSROOT);

    manager->SetIOSettings(
        ioSettings);


    // ========================================
    // Scene
    // ========================================

    FbxScene* scene =
        FbxScene::Create(
            manager,
            "Scene");


    // ========================================
    // Importer
    // ========================================

    FbxImporter* importer =
        FbxImporter::Create(
            manager,
            "Importer");

    if (!importer->Initialize(
        inputPath.c_str(),
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


    if (!importer->Import(scene))
    {
        std::cerr
            << "FBX Import失敗\n";

        importer->Destroy();
        manager->Destroy();

        return 1;
    }

    importer->Destroy();


    // ========================================
    // 三角形化
    // ========================================

    FbxGeometryConverter converter(
        manager);

    converter.Triangulate(
        scene,
        true);


    // ========================================
    // Mesh抽出
    // ========================================

    std::vector<BRVertex> vertices;

    std::vector<uint32_t> indices;


    FbxNode* root =
        scene->GetRootNode();

    if (root)
    {
        ReadNode(
            root,
            vertices,
            indices);
    }


    std::cout
        << "Vertex : "
        << vertices.size()
        << "\n";

    std::cout
        << "Index  : "
        << indices.size()
        << "\n";


    // ========================================
    // 保存
    // ========================================

    if (!SaveBRModel(
        outputPath,
        vertices,
        indices))
    {
        manager->Destroy();

        return 1;
    }


    std::cout
        << "Convert成功\n";

    std::cout
        << inputPath
        << "\n -> "
        << outputPath
        << "\n";


    manager->Destroy();

    return 0;
}