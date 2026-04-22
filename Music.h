#pragma once
#include "Engine/GameObject.h"
#include<string>

//◆◆◆を管理するクラス
class Music : public GameObject
{
public:
    //コンストラクタ
    Music(GameObject* parent);

    //デストラクタ
    ~Music();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

    bool IsStarted()const { return started_; }
    double GetNowSec()const { return nowSec_; }
    void SelectedMusic();

private:
    int hSound_;
    bool started_;
    double nowSec_;
    std::string musicName;

};
