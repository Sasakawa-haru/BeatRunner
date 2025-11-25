#include "Stage.h"
#include"Lane.h"
#include"Engine/CsvReader.h"
#include"Engine/Model.h"
#include"Engine/Camera.h"

const int LaneCount = 5;

//コンストラクタ
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hStageModel_(-1)
{
}

//デストラクタ
Stage::~Stage()
{
}

//初期化
void Stage::Initialize()
{
    hStageModel_ = Model::Load("Models/Stage.fbx");
	assert(hStageModel_ >= 0);

    Lane::ResetLaneIndex();

    for (int i = 0; i < LaneCount; i++) {
        Instantiate<Lane>(this);
    }
}

//更新
void Stage::Update()
{
}

//描画
void Stage::Draw()
{
    Model::SetTransform(hStageModel_, transform_);
    Model::Draw(hStageModel_);

    Camera::SetPosition(XMFLOAT3(0, 5, -15));

    Camera::SetTarget(XMFLOAT3(0, 1, 0));
}

//開放
void Stage::Release()
{
}