#include "Lane.h"
#include"Engine/Model.h"
#include"Engine/Camera.h"

int Lane::sNextLaneIndex_ = 0;
const int laneCount = 5;
const float laneWidth = 2.0f;

//コンストラクタ
Lane::Lane(GameObject* parent)
    :GameObject(parent, "Lane"),hLaneModel_(-1)
{
    laneIndex_ = sNextLaneIndex_++;
}

//デストラクタ
Lane::~Lane()
{
}

//初期化
void Lane::Initialize()
{
    hLaneModel_ = Model::Load("Models/Lane.fbx");
    assert(hLaneModel_ >= 0);
    float offset = (laneIndex_ - (laneCount - 1) * 0.5f) * laneWidth;
    transform_.position_.x = offset;
    transform_.position_.y = 1.0f;
	transform_.position_.z = 0.0f;
}

//更新
void Lane::Update()
{
}

//描画
void Lane::Draw()
{
	Model::SetTransform(hLaneModel_, transform_);
	Model::Draw(hLaneModel_);
}

//開放
void Lane::Release()
{
}