#include "Global.h"
#include "Model.h"

// 3Dモデル（BRMファイル）を管理する

namespace Model
{
    // ロード済みのモデルデータ一覧
    std::vector<ModelData*> _datas;


    // =========================================
    // 初期化
    // =========================================

    void Initialize()
    {
        AllRelease();
    }


    // =========================================
    // モデルをロード
    // =========================================

    int Load(std::string fileName)
    {
        ModelData* pData =
            new ModelData;


        // ファイル名を保存
        pData->fileName =
            fileName;


        // =====================================
        // 同じモデルがすでにロード済みか確認
        // =====================================

        bool isExist = false;


        for (int i = 0;
            i < static_cast<int>(_datas.size());
            ++i)
        {
            if (_datas[i] != nullptr
                && _datas[i]->fileName == fileName)
            {
                // すでにロード済みなら共有
                pData->pModel =
                    _datas[i]->pModel;

                isExist = true;

                break;
            }
        }


        // =====================================
        // 新しくロード
        // =====================================

        if (!isExist)
        {
            pData->pModel =
                new BRModel;


            if (!pData->pModel->Load(
                fileName))
            {
                SAFE_DELETE(
                    pData->pModel
                );

                SAFE_DELETE(
                    pData
                );

                return -1;
            }
        }


        // =====================================
        // 空いているHandleを探す
        // =====================================

        for (int i = 0;
            i < static_cast<int>(_datas.size());
            ++i)
        {
            if (_datas[i] == nullptr)
            {
                _datas[i] =
                    pData;

                return i;
            }
        }


        // =====================================
        // 新しいHandle
        // =====================================

        _datas.push_back(
            pData
        );


        return static_cast<int>(
            _datas.size() - 1
            );
    }


    // =========================================
    // 描画
    // =========================================

    void Draw(int handle)
    {
        if (handle < 0
            || handle >= static_cast<int>(_datas.size())
            || _datas[handle] == nullptr)
        {
            return;
        }


        ModelData* data =
            _datas[handle];


        // =====================================
        // アニメーションを進める
        // =====================================

        data->nowFrame +=
            data->animSpeed;


        // =====================================
        // 終端まで行ったら開始Frameへ
        // =====================================

        if (data->nowFrame >
            static_cast<float>(
                data->endFrame
                ))
        {
            data->nowFrame =
                static_cast<float>(
                    data->startFrame
                    );
        }


        // =====================================
        // BRModel描画
        // =====================================

        if (data->pModel)
        {
            data->pModel->Draw(
                data->transform,
                static_cast<int>(
                    data->nowFrame
                    )
            );
        }
    }


    // =========================================
    // 任意モデル解放
    // =========================================

    void Release(int handle)
    {
        if (handle < 0
            || handle >= static_cast<int>(_datas.size())
            || _datas[handle] == nullptr)
        {
            return;
        }


        // =====================================
        // 同じBRModelを別Handleが使っているか
        // =====================================

        bool isExist = false;


        for (int i = 0;
            i < static_cast<int>(_datas.size());
            ++i)
        {
            if (_datas[i] != nullptr
                && i != handle
                && _datas[i]->pModel
                == _datas[handle]->pModel)
            {
                isExist = true;

                break;
            }
        }


        // 他で使っていなければモデル本体を削除
        if (!isExist)
        {
            SAFE_DELETE(
                _datas[handle]->pModel
            );
        }


        SAFE_DELETE(
            _datas[handle]
        );
    }


    // =========================================
    // 全モデル解放
    // =========================================

    void AllRelease()
    {
        for (int i = 0;
            i < static_cast<int>(_datas.size());
            ++i)
        {
            if (_datas[i] != nullptr)
            {
                Release(i);
            }
        }


        _datas.clear();
    }


    // =========================================
    // Animation設定
    // =========================================

    void SetAnimFrame(
        int handle,
        int startFrame,
        int endFrame,
        float animSpeed)
    {
        if (handle < 0
            || handle >= static_cast<int>(_datas.size())
            || _datas[handle] == nullptr)
        {
            return;
        }


        _datas[handle]->SetAnimFrame(
            startFrame,
            endFrame,
            animSpeed
        );
    }


    // =========================================
    // 現在Frame取得
    // =========================================

    int GetAnimFrame(
        int handle)
    {
        if (handle < 0
            || handle >= static_cast<int>(_datas.size())
            || _datas[handle] == nullptr)
        {
            return 0;
        }


        return static_cast<int>(
            _datas[handle]->nowFrame
            );
    }


    // =========================================
    // Bone位置
    //
    // 現在はBRModel側未実装
    // =========================================

    XMFLOAT3 GetBonePosition(
        int handle,
        std::string boneName)
    {
        return XMFLOAT3(
            0.0f,
            0.0f,
            0.0f
        );
    }


    // =========================================
    // Transform設定
    // =========================================

    void SetTransform(
        int handle,
        Transform& transform)
    {
        if (handle < 0
            || handle >= static_cast<int>(_datas.size())
            || _datas[handle] == nullptr)
        {
            return;
        }


        _datas[handle]->transform =
            transform;
    }


    // =========================================
    // WorldMatrix取得
    // =========================================

    XMMATRIX GetMatrix(
        int handle)
    {
        if (handle < 0
            || handle >= static_cast<int>(_datas.size())
            || _datas[handle] == nullptr)
        {
            return XMMatrixIdentity();
        }


        return
            _datas[handle]
            ->transform
            .GetWorldMatrix();
    }


    // =========================================
    // RayCast
    //
    // 現在BRModelでは未実装
    // =========================================

    void RayCast(
        int handle,
        RayCastData* data)
    {
        if (!data)
        {
            return;
        }


        data->hit =
            FALSE;
    }
}