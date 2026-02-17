#include "PlayScene.h"
#include"Player.h"
#include"Lane.h"
#include"Notes.h"
#include"Music.h"
#include"JudgeSystem.h"
#include"Engine/Input.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"


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
	Instantiate<Player>(this);
}

//更新
void PlayScene::Update()
{
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