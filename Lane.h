#pragma once
#include "Engine/GameObject.h"
#include<string>
#include<map>
#include"Engine/Model.h"

enum class LaneType {
    Lane1,
    Lane2,
    Lane3,
    Lane4,
    Lane5,
    Unknown
};

class Lane : public GameObject
{

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

    XMFLOAT3 GetCenterPosition()const;
    LaneType GetLaneType() const { return laneType_; }
    const std::string& GetLaneName()const {return laneName_;}
    static Lane* FindByName(const std::string& name);
    
   

	static void ResetLaneIndex() { sNextLaneIndex_ = 0; }
    int GetLaneHandle() { return hLaneModel_; }

private:
    int hLaneModel_;
    static int sNextLaneIndex_;
    int laneIndex_;
    LaneType laneType_;

    std::string laneName_;
    static std::_In_place_key_extract_map < std::string, Lane*>sLaneTable_;


};