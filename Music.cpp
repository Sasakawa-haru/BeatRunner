#include "Music.h"
#include"Engine/Audio.h"

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
    if (!started_) {
        hSound_ = Audio::Load("Sound/01 - Chartreuse Green.wav");
        started_ = true;
    }

}

//更新
void Music::Update()
{

    Audio::Play(hSound_);
}

//描画
void Music::Draw()
{
}

//開放
void Music::Release()
{
}