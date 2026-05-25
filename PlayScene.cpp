#include "PlayScene.h"
#include"Player.h"
#include"Lane.h"
#include"Notes.h"
#include"Music.h"
#include"JudgeSystem.h"
#include"ScoreSystem.h"
#include"ResultData.h"
#include"Engine/Input.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"
#include"SelectedMusic.h"


//コンストラクタ
PlayScene::PlayScene(GameObject* parent)
	: GameObject(parent, "PlayScene")
{
}

//初期化
void PlayScene::Initialize()
{
	Lane::ResetLaneIndex();

	for (int i = 0; i < Lane::laneCount; i++) {
		Instantiate<Lane>(this);
	}
	Instantiate<Music>(this);
	Instantiate<Notes>(this);
	Instantiate<JudgeSystem>(this);
	Instantiate<ScoreSystem>(this);
	Instantiate<Player>(this);
}

//更新
void PlayScene::Update()
{
	Player* player = (Player*)FindObject("Player");
	if (player) {
		int hp = player->GetHP();
		if (hp <= 0) {

		}
	}
	char buf[64];
	sprintf_s(buf, "dt=%.6f\n", Time::DeltaTime());
	OutputDebugStringA(buf);
	if (Input::IsKeyDown(DIK_ESCAPE)) {
	}
	
}

//描画
void PlayScene::Draw()
{
}

//開放
void PlayScene::Release()
{
}

void PlayScene::SaveResultData() 
{
	ScoreSystem* scoreSystem = (ScoreSystem*)FindObject("ScoreSystem");
	Player* player = (Player*)FindObject("Player");
	if (scoreSystem) {
		gResultData.score = scoreSystem->GetScore();
		gResultData.maxCombo = scoreSystem->GetMaxCombo();

		gResultData.perfect = scoreSystem->GetPerfect();
		gResultData.great = scoreSystem->GetGreat();
		gResultData.good = scoreSystem->GetGood();
		gResultData.normal = scoreSystem->GetNormal();
		gResultData.miss = scoreSystem->GetMiss();
	}

}