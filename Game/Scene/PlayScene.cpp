#include "Game/Scene/PlayScene.h"
#include"Game/Player/Player.h"
#include"Game/Rhythm/Lane/Lane.h"
#include"Game/Rhythm/Note/Notes.h"
#include"Game/Rhythm/Music/Music.h"
#include"Game/Rhythm/Judge/JudgeLine.h"
#include"Game/Stage/SideObject.h"
#include"Game/Rhythm/Judge/JudgeSystem.h"
#include"Game/Rhythm/Score/ScoreSystem.h"
#include"Game/Result/ResultData.h"
#include"Engine/Input.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"
#include"Engine/Image.h"
#include"Engine/SceneManager.h"
#include"Game/Rhythm/Music/SelectedMusic.h"
#include"Game/Config/GameConfig.h"
#include <iostream>

#include"Engine/BRModel.h"


//コンストラクタ
PlayScene::PlayScene(GameObject* parent)
	: GameObject(parent, "PlayScene")
{
}

//初期化
void PlayScene::Initialize()
{

	const bool loaded = gameConfig_.LoadFromCsv("Assets/Csv/game_config.csv");
	if (!loaded)
	{
		std::cout << "game_config.csvの読み込みに失敗しました" << std::endl;
	}

	Lane::laneCount = gameConfig_.GetLaneCount();
	Lane::laneWidth = gameConfig_.GetLaneWidth();

	Lane::ResetLaneIndex();

	for (int i = 0; i < Lane::laneCount; i++) {
		Instantiate<Lane>(this);
	}
	Instantiate<JudgeLine>(this);
	Instantiate<Music>(this);
	Notes*notes=Instantiate<Notes>(this);
	Instantiate<Player>(this);
	Instantiate<SideObject>(this);

	Instantiate<JudgeSystem>(this);
	ScoreSystem* scoreSystem=Instantiate<ScoreSystem>(this);
	if (notes && scoreSystem) {
		scoreSystem->SetScoreMaxCombo(notes->GetAllNotesCount());
	}
}

//更新
void PlayScene::Update()
{
	float dt = Time::DeltaTime();
	if (dt <= 0.0f || dt > 0.05f){
		dt = 1.0f / 60.0f;
	}

	Music* music = (Music*)FindObject("Music");
	Player* player = (Player*)FindObject("Player");
	SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");

	switch(clearState_)
		case ClearState::Playing:
			if (music && music->IsMusicFinished())
			{
				clearState_ = ClearState::PlayerPerformance;
				clearTimer_ = 0.0f;
				if (player) {
					player->StartClearPerformance();
				}
				break;
		case ClearState::PlayerPerformance:
			if (player && player->UpdateClearPerformance(dt))
			{
				clearState_ = ClearState::ShowClear;
				clearTimer_ = 0.0f;
			}
			break;

		case ClearState::ShowClear:
			clearTimer_ += dt;

			if (clearTimer_ >= clearShowTime_)
			{
				SaveResultData();

				SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
				if (pSceneManager)
				{
					pSceneManager->ChangeScene(SCENE_ID_CLEAR);
				}

				clearState_ = ClearState::ChangeResult;
			}
			break;

		case ClearState::ChangeResult:
			break;
			
			}

	if (player) {
		int hp = player->GetHP();
		if (hp <= 0) {
			SaveResultData();
			pSceneManager->ChangeScene(SCENE_ID_RESULT);
		}
	}
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
	if (player)
	{
		gResultData.hp = player->GetHP();
	}

}