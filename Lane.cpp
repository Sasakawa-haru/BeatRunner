#include "Lane.h"
#include "Engine/Model.h"
#include "Notes.h"
#include"Music.h"
#include <cassert>
#include <map>

int Lane::sNextLaneIndex_ = 0;
const int Lane::laneCount = 5;
const float Lane::laneWidth = 2.0f;

std::map<std::string, Lane*> Lane::sLaneTable_;


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
    case 5: laneType_ = LaneType::Lane6; break;
    case 6: laneType_ = LaneType::Lane7; break;
    default: laneType_ = LaneType::Unknown; break;
    }

    laneName_ = "lane" + std::to_string(laneIndex_ + 1);

    sLaneTable_[laneName_] = this;
}

Lane::~Lane()
{
    sLaneTable_.erase(laneName_);
}

void Lane::Initialize()
{
    hLaneModel_ = Model::Load("Models/Lane.fbx");
    assert(hLaneModel_ >= 0);

    float offset = (laneIndex_ - (laneCount+1) * 0.5f) * laneWidth;
    transform_.position_.x = offset;
    transform_.position_.y = -1.0f;
    transform_.position_.z = 0.0f;

    Instantiate<Music>(this);

}

void Lane::Update()
{
}

void Lane::Draw()
{
    Transform t = transform_;
    t.position_.x += laneWidth * 0.5f; 
    Model::SetTransform(hLaneModel_, t);
    Model::Draw(hLaneModel_);
}

void Lane::Release()
{
}


void Lane::ResetLaneIndex()
{
    sNextLaneIndex_ = 0;
    sLaneTable_.clear();
}

XMFLOAT3 Lane::GetCenterPosition() const
{
    //XMFLOAT3 p = transform_.position_;
    //p.x += laneWidth / 2;
    return transform_.position_;
}

Lane* Lane::FindByName(const std::string& name)
{
    auto it = sLaneTable_.find(name);
    if (it != sLaneTable_.end()) {
        return it->second;
    }
    return nullptr;
}
