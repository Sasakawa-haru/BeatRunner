#include "Lane.h"
#include "Engine/Model.h"
#include <cassert>
#include <map>

int Lane::sNextLaneIndex_ = 0;
std::map<std::string, Lane*> Lane::sLaneTable_;

const int laneCount = 5;
const float laneWidth = 2.0f;

Lane::Lane(GameObject* parent)
    : GameObject(parent, "Lane")
    , hLaneModel_(-1)
    , laneType_(LaneType::Unknown)
{
    laneIndex_ = sNextLaneIndex_++;

    switch (laneIndex_) {
    case 0: laneType_ = LaneType::Lane1; break;
    case 1: laneType_ = LaneType::Lane2; break;
    case 2: laneType_ = LaneType::Lane3; break;
    case 3: laneType_ = LaneType::Lane4; break;
    case 4: laneType_ = LaneType::Lane5; break;
    default: laneType_ = LaneType::Unknown; break;
    }

    // ★ lane1 / lane2 / lane3 … という名前を付ける（1始まり）
    laneName_ = "lane" + std::to_string(laneIndex_ + 1);

    // ★ 静的テーブルに登録
    sLaneTable_[laneName_] = this;
}

Lane::~Lane()
{
    // ★ 破棄時はテーブルから消す
    sLaneTable_.erase(laneName_);
}

void Lane::Initialize()
{
    hLaneModel_ = Model::Load("Models/Lane.fbx");
    assert(hLaneModel_ >= 0);

    float offset = (laneIndex_ - (laneCount - 1) * 0.5f) * laneWidth;
    transform_.position_.x = offset;
    transform_.position_.y = -1.0f;
    transform_.position_.z = 0.0f;
}

void Lane::Update()
{
}

void Lane::Draw()
{
    Model::SetTransform(hLaneModel_, transform_);
    Model::Draw(hLaneModel_);
}

void Lane::Release()
{
}

XMFLOAT3 Lane::GetCenterPosition() const
{
    return transform_.position_;
}

// ★ 追加：名前から Lane を取得
Lane* Lane::FindByName(const std::string& name)
{
    auto it = sLaneTable_.find(name);
    if (it != sLaneTable_.end()) {
        return it->second;
    }
    return nullptr;
}
