#pragma once
#include "Engine/GameObject.h"
#include"Engine/Model.h"

//◆◆◆を管理するクラス
class Lane : public GameObject
{
    int hLaneModel_;
    static int sNextLaneIndex_;
    int laneIndex_;

public:

    //コンストラクタ
    Lane(GameObject* parent);

    //デストラクタ
    ~Lane();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

	static void ResetLaneIndex() { sNextLaneIndex_ = 0; }
	int GetLaneIndex() { return laneIndex_; }

};