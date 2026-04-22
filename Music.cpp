#include "Music.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"
#include"SelectedMusic.h"

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
    SelectedMusic();
    started_ = false;
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

void Music::SelectedMusic()
{
    musicName = gSelectedMusicName;
    std::string MusicPath = "Sound_" + musicName + ".wav";
    hSound_ = Audio::Load(MusicPath.c_str());
}