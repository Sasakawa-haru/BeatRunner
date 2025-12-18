#include "PlayScene.h"
#include"Player.h"
#include"Lane.h"
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

	Instantiate<Player>(this);
}

//更新
void PlayScene::Update()
{
	char buf[64];
	sprintf_s(buf, "dt=%.6f\n", Time::DeltaTime());
	OutputDebugStringA(buf);
}

//描画
void PlayScene::Draw()
{
}

//開放
void PlayScene::Release()
{
}