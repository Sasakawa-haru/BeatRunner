#include "PlayScene.h"
#include"Player.h"
#include"Lane.h"

const int LaneCount = 5;

//コンストラクタ
PlayScene::PlayScene(GameObject* parent)
	: GameObject(parent, "PlayScene")
{
}

//初期化
void PlayScene::Initialize()
{
	Lane::ResetLaneIndex();

	for (int i = 0; i < LaneCount; i++) {
		Instantiate<Lane>(this);
	}

	Instantiate<Player>(this);
}

//更新
void PlayScene::Update()
{
}

//描画
void PlayScene::Draw()
{
}

//開放
void PlayScene::Release()
{
}