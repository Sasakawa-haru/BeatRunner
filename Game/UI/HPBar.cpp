#include "Game/UI/HPBar.h"

#include "Game/Player/Player.h"
#include "Engine/Image.h"

#include <algorithm>
#include <cassert>

HPBar::HPBar(GameObject* parent)
    : GameObject(parent, "HPBar")
{
}

HPBar::~HPBar()
{
}

void HPBar::Initialize()
{
    player_ = static_cast<Player*>(FindObject("Player"));

    hBack_ = Image::Load("UI/HPBarBack.png");
    hGauge_ = Image::Load("UI/HPBar.png");

    assert(hBack_ != INVALID_HANDLE);
    assert(hGauge_ != INVALID_HANDLE);
}

void HPBar::Update()
{
    // Playerがまだ見つかっていなければ再検索
    if (player_ == nullptr)
    {
        player_ = static_cast<Player*>(FindObject("Player"));

        if (player_ == nullptr)
        {
            return;
        }
    }

    const int hp = player_->GetHP();
    const int maxHP = player_->GetMaxHP();

    if (maxHP <= 0)
    {
        hpRate_ = MIN_HP_RATE;
        return;
    }

    hpRate_ =
        static_cast<float>(hp) /
        static_cast<float>(maxHP);

    hpRate_ = std::clamp(
        hpRate_,
        MIN_HP_RATE,
        MAX_HP_RATE
    );
}

void HPBar::Draw()
{
    Transform backTransform;

    backTransform.position_.x = BAR_POS_X;
    backTransform.position_.y = BAR_POS_Y;
    backTransform.position_.z = BAR_POS_Z;

    backTransform.scale_.x = BAR_SCALE_X;
    backTransform.scale_.y = BAR_SCALE_Y;
    backTransform.scale_.z = BAR_SCALE_Z;

    Image::SetTransform(hBack_, backTransform);
    Image::Draw(hBack_);

    const int drawWidth =
        static_cast<int>(
            static_cast<float>(GAUGE_WIDTH) * hpRate_
            );

    if (drawWidth <= 0)
    {
        return;
    }

    Image::SetRect(
        hGauge_,
        0,
        0,
        drawWidth,
        GAUGE_HEIGHT
    );

    Transform gaugeTransform;

    // HP減少で消えたピクセル数
    const float removedWidth =
        static_cast<float>(GAUGE_WIDTH - drawWidth);

    // 1280幅の画面座標へ変換
    const float offsetX =
        removedWidth / SCREEN_WIDTH;

    // ★ 左端固定用
    gaugeTransform.position_.x =
        BAR_POS_X - offsetX;

    gaugeTransform.position_.y = BAR_POS_Y;
    gaugeTransform.position_.z = BAR_POS_Z;

    gaugeTransform.scale_.x = BAR_SCALE_X;
    gaugeTransform.scale_.y = BAR_SCALE_Y;
    gaugeTransform.scale_.z = BAR_SCALE_Z;

    Image::SetTransform(hGauge_, gaugeTransform);
    Image::Draw(hGauge_);
}
void HPBar::Release()
{
}