#pragma once

#include "Engine/GameObject.h"
#include "Engine/Model.h"      
#include <string>
#include <map>


// レーン種類
enum class LaneType
{
    Lane1,
    Lane2,
    Lane3,
    Lane4,
    Lane5,
    Unknown,
};

class Lane : public GameObject
{
public:
    static const int laneCount;
    static const float laneWidth;

    Lane(GameObject* parent);
    ~Lane();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    static void ResetLaneIndex();


    // レーン中心（= Lane の transform_.position_）
    XMFLOAT3 GetCenterPosition() const;

    // モデルハンドル（PlayerのRayCast用に必要なら）
    int GetLaneHandle() const { return hLaneModel_; }

    // レーン種別
    LaneType GetLaneType() const { return laneType_; }

    // ★ lane1/lane2... の名前
    const std::string& GetLaneName() const { return laneName_; }

    // ★ "lane2" みたいな名前でレーンを取得
    static Lane* FindByName(const std::string& name);

private:
    // 次に作られる Lane の index を決める（0,1,2,...）
    static int sNextLaneIndex_;

    // ★ 名前→Lane* の辞書
    static std::map<std::string, Lane*> sLaneTable_;

    int laneIndex_ = 0;
    LaneType laneType_ = LaneType::Unknown;

    int hLaneModel_ = -1;

    std::string laneName_;
};
