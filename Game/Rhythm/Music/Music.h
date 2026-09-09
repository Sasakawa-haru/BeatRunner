#pragma once
#include "Engine/GameObject.h"
#include<string>

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
    void LoadSelectedMusic();

    bool IsMusicFinished()const { return finished_; }

    bool IsWaiting()const {
        return !started_ && startWaitTimer_ < startWaitTime_;
    }

    float GetStartWaitRemaining()const {
        float remaining = startWaitTime_ - startWaitTimer_;
        if (remaining < 0.0f) {
            remaining=0.0f;
        }
        return remaining;
    }

    double GetStartWaitTime()const {
        return startWaitTime_;
    }

private:
    int hSound_;
    bool started_;
    bool finished_;
    double nowSec_;
    std::string musicName;

    double startWaitTimer_ = 0.0;
    double startWaitTime_ = 3.0;

};