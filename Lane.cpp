#include "Lane.h"
#include "Engine/Model.h"
#include "Engine/Camera.h"
#include <cassert>

int Lane::sNextLaneIndex_ = 0;
const float laneWidth = 2.0f;

// ★ "Lane0", "Lane1", ... を作るヘルパー
std::string Lane::MakeLaneName(int index)
{
    return "Lane" + std::to_string(index);
}

// コンストラクタ
Lane::Lane(GameObject* parent)
    : GameObject(parent, MakeLaneName(sNextLaneIndex_))  // ← ここでユニーク名
    , hLaneModel_(-1)
    , laneIndex_(sNextLaneIndex_)
    , laneType_(LaneType::Unknown)
{
    ++sNextLaneIndex_;

    switch (laneIndex_) {
    case 0: laneType_ = LaneType::Lane1; break;
    case 1: laneType_ = LaneType::Lane2; break;
    case 2: laneType_ = LaneType::Lane3; break;
    case 3: laneType_ = LaneType::Lane4; break;
    case 4: laneType_ = LaneType::Lane5; break;
    default: laneType_ = LaneType::Unknown; break;
    }
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
    transform_.position_.y = -1.0f;
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

XMFLOAT3 Lane::GetCenterPosition() const
{
    return transform_.position_;
}

LaneType Lane::GetLaneType() const
{
    return laneType_;
}
