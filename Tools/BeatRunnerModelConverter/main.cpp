#include <fbxsdk.h>

#include "Common/BRModelFormat.h"

#include <Windows.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


// ============================================
// UTF-8に
// ============================================

std::string WideToUtf8(const std::wstring& text)
{
    if (text.empty())
    {
        return {};
    }

    const int requiredSize =
        WideCharToMultiByte(
            CP_UTF8,
            0,
            text.c_str(),
            -1,
            nullptr,
            0,
            nullptr,
            nullptr
        );

    if (requiredSize <= 0)
    {
        return {};
    }

    // WideCharToMultiByte() は終端\0を含むサイズを返す
    std::string utf8(
        static_cast<size_t>(requiredSize),
        '\0'
    );

    WideCharToMultiByte(
        CP_UTF8,
        0,
        text.c_str(),
        -1,
        utf8.data(),
        requiredSize,
        nullptr,
        nullptr
    );

    // std::string自身が終端を持つため、
    // 変換結果末尾の\0を削除
    if (!utf8.empty() && utf8.back() == '\0')
    {
        utf8.pop_back();
    }

    return utf8;
}


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


    // ========================================
    // UVセット名取得
    // ========================================

    FbxStringList uvSetNames;

    mesh->GetUVSetNames(uvSetNames);

    const char* uvSetName = nullptr;

    if (uvSetNames.GetCount() > 0)
    {
        uvSetName =
            uvSetNames.GetStringAt(0);
    }


    // ========================================
    // Polygon読み込み
    // ========================================

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
                    polygonVertex
                );

            const FbxVector4 position =
                mesh->GetControlPointAt(
                    controlPointIndex
                );

            vertex.px =
                static_cast<float>(
                    position[0]
                    );

            vertex.py =
                static_cast<float>(
                    position[1]
                    );

            vertex.pz =
                static_cast<float>(
                    position[2]
                    );


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
                    static_cast<float>(
                        normal[0]
                        );

                vertex.ny =
                    static_cast<float>(
                        normal[1]
                        );

                vertex.nz =
                    static_cast<float>(
                        normal[2]
                        );
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
                        static_cast<float>(
                            uv[0]
                            );

                    // DirectX用にV反転
                    vertex.v =
                        1.0f -
                        static_cast<float>(
                            uv[1]
                            );
                }
            }


            // =================================
            // 頂点・Index追加
            // =================================

            const uint32_t index =
                static_cast<uint32_t>(
                    vertices.size()
                    );

            vertices.push_back(vertex);

            indices.push_back(index);
        }
    }

    return true;
}


// ============================================
// Sceneのノードを再帰検索
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
                indices
            );
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
            indices
        );
    }
}


// ============================================
// BRM保存
// ============================================

bool SaveBRModel(
    const std::filesystem::path& filePath,
    const std::vector<BRVertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    // 出力先フォルダが無ければ作る
    const std::filesystem::path parent =
        filePath.parent_path();

    if (!parent.empty())
    {
        std::error_code ec;

        std::filesystem::create_directories(
            parent,
            ec
        );

        if (ec)
        {
            std::wcerr
                << L"出力フォルダを作成できません: "
                << parent.wstring()
                << L"\n";

            return false;
        }
    }


    std::ofstream file(
        filePath,
        std::ios::binary
    );

    if (!file)
    {
        std::wcerr
            << L"出力ファイルを作成できません: "
            << filePath.wstring()
            << L"\n";

        return false;
    }


    // ========================================
    // Header
    // ========================================

    BRModelHeader header{};

    header.magic[0] = 'B';
    header.magic[1] = 'R';
    header.magic[2] = 'M';
    header.magic[3] = '1';

    header.version = 1;

    header.vertexCount =
        static_cast<uint32_t>(
            vertices.size()
            );

    header.indexCount =
        static_cast<uint32_t>(
            indices.size()
            );


    file.write(
        reinterpret_cast<const char*>(
            &header
            ),
        sizeof(header)
    );


    // ========================================
    // Vertex
    // ========================================

    if (!vertices.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                vertices.data()
                ),
            sizeof(BRVertex)
            * vertices.size()
        );
    }


    // ========================================
    // Index
    // ========================================

    if (!indices.empty())
    {
        file.write(
            reinterpret_cast<const char*>(
                indices.data()
                ),
            sizeof(uint32_t)
            * indices.size()
        );
    }


    if (!file)
    {
        std::cerr
            << "BRM書き込み中にエラーが発生しました\n";

        return false;
    }


    return true;
}


// ============================================
// main
// ============================================

int wmain(
    int argc,
    wchar_t* argv[])
{
    // コンソール出力をUTF-8にする
    SetConsoleOutputCP(CP_UTF8);


    if (argc < 3)
    {
        std::wcout
            << L"Usage:\n"
            << L"BeatRunnerModelConverter.exe input.fbx output.brm\n";

        return 0;
    }


    // ========================================
    // Windows側ではUTF-16でパスを保持
    // ========================================

    const std::wstring inputPathW =
        argv[1];

    const std::wstring outputPathW =
        argv[2];


    std::wcout
        << L"Input : "
        << inputPathW
        << L"\n";

    std::wcout
        << L"Output: "
        << outputPathW
        << L"\n";


    // ========================================
    // 入力ファイル確認
    // ========================================

    const std::filesystem::path inputPath(
        inputPathW
    );

    const std::filesystem::path outputPath(
        outputPathW
    );


    if (!std::filesystem::exists(inputPath))
    {
        std::wcerr
            << L"入力FBXが存在しません:\n"
            << inputPathW
            << L"\n";

        return 1;
    }


    // ========================================
    // FBX SDK用にUTF-8へ変換
    // ========================================

    const std::string inputPathUtf8 =
        WideToUtf8(inputPathW);

    if (inputPathUtf8.empty())
    {
        std::cerr
            << "入力パスのUTF-8変換に失敗しました\n";

        return 1;
    }


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


    // ========================================
    // IO Settings
    // ========================================

    FbxIOSettings* ioSettings =
        FbxIOSettings::Create(
            manager,
            IOSROOT
        );

    if (!ioSettings)
    {
        std::cerr
            << "FbxIOSettings作成失敗\n";

        manager->Destroy();

        return 1;
    }


    manager->SetIOSettings(
        ioSettings
    );


    // ========================================
    // Scene
    // ========================================

    FbxScene* scene =
        FbxScene::Create(
            manager,
            "Scene"
        );

    if (!scene)
    {
        std::cerr
            << "FbxScene作成失敗\n";

        manager->Destroy();

        return 1;
    }


    // ========================================
    // Importer
    // ========================================

    FbxImporter* importer =
        FbxImporter::Create(
            manager,
            "Importer"
        );

    if (!importer)
    {
        std::cerr
            << "FbxImporter作成失敗\n";

        manager->Destroy();

        return 1;
    }


    // FBX SDKはUTF-8文字列を使用する
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


    // ========================================
    // Import
    // ========================================

    if (!importer->Import(scene))
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


    // ========================================
    // 三角形化
    // ========================================

    FbxGeometryConverter converter(
        manager
    );

    if (!converter.Triangulate(
        scene,
        true))
    {
        std::cerr
            << "三角形化に失敗しました\n";

        manager->Destroy();

        return 1;
    }


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
            indices
        );
    }


    // ========================================
    // 結果表示
    // ========================================

    std::cout
        << "Vertex : "
        << vertices.size()
        << "\n";

    std::cout
        << "Index  : "
        << indices.size()
        << "\n";


    if (vertices.empty())
    {
        std::cerr
            << "Meshが見つかりませんでした\n";

        manager->Destroy();

        return 1;
    }


    // ========================================
    // BRM保存
    // ========================================

    if (!SaveBRModel(
        outputPath,
        vertices,
        indices))
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