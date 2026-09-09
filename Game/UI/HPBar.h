#pragma once

#include "Engine/GameObject.h"

class Player;

class HPBar : public GameObject
{
public:
    HPBar(GameObject* parent);
    ~HPBar();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

private:
    Player* player_ = nullptr;

    int hBack_ = -1;
    int hGauge_ = -1;

    float hpRate_ = 1.0f;

    static constexpr float SCREEN_WIDTH = 1280.0f;

    // -------------------------
    // HPバー画像サイズ
    // -------------------------
    static constexpr int GAUGE_WIDTH = 400;
    static constexpr int GAUGE_HEIGHT = 30;

    // -------------------------
    // HPバー表示位置
    // -------------------------
    static constexpr float BAR_POS_X = -0.65f;
    static constexpr float BAR_POS_Y = 0.80f;
    static constexpr float BAR_POS_Z = 0.0f;

    // -------------------------
    // スケール
    // -------------------------
    static constexpr float BAR_SCALE_X = 1.0f;
    static constexpr float BAR_SCALE_Y = 1.0f;
    static constexpr float BAR_SCALE_Z = 1.0f;

    // -------------------------
    // HP割合
    // -------------------------
    static constexpr float MIN_HP_RATE = 0.0f;
    static constexpr float MAX_HP_RATE = 1.0f;

    // -------------------------
    // 画像ハンドル初期値
    // -------------------------
    static constexpr int INVALID_HANDLE = -1;

    // ゲージ画像の切り抜き開始位置
    static constexpr int GAUGE_RECT_X = 0;
    static constexpr int GAUGE_RECT_Y = 0;
};