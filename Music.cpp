#include "Music.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"

//コンストラクタ
Music::Music(GameObject* parent)
    :GameObject(parent, "Music")
{
}

//デストラクタ
Music::~Music()
{
}

//初期化
void Music::Initialize()
{
    hSound_ = Audio::Load("Sound/01 - Chartreuse Green.wav");
    started_ = true;
    nowSec_ = 0.0f;
    

}

//更新
void Music::Update()
{
    if (!started_) {
        Audio::Play(hSound_);
        started_ = true;
        nowSec_=0.0;
        return;
    }

    nowSec_ += Time::DeltaTime();
}

//描画
void Music::Draw()
{
}

//開放
void Music::Release()
{
}