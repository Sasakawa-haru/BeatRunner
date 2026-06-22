#include "Music.h"
#include "OptionData.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"
#include"Engine/SceneManager.h"
#include"SelectedMusic.h"

//コンストラクタ
Music::Music(GameObject* parent)
    :GameObject(parent, "Music"),
    hSound_(-1),
    started_(false),
    finished_(false),
    nowSec_(0.0f)
{
}

//デストラクタ
Music::~Music()
{
}

//初期化
void Music::Initialize()
{
    LoadSelectedMusic();
    started_ = false;
    finished_ = false;
    nowSec_ = 0.0f;
    

}

//更新
void Music::Update()
{
    if (!started_) {
        Audio::Play(hSound_);
        started_ = true;
        nowSec_=0.0f;
        return;
    }

    nowSec_ += Time::DeltaTime();

    if (!Audio::IsPlaying(hSound_)) {//曲終了時
        started_ = false;
        finished_ = true;
    }
}

//描画
void Music::Draw()
{
}

//開放
void Music::Release()
{
}

void Music::LoadSelectedMusic()
{
    musicName = gSelectedMusicName;
    std::string MusicPath = "Sound/Music/" + musicName + ".wav";
    hSound_ = Audio::Load(MusicPath.c_str());
    if (hSound_ < 0) {
        started_ = true;
        return;
    }
    Audio::SetVolume(hSound_, gOptionData.MusicVolume * gOptionData.MasterVolume);
}