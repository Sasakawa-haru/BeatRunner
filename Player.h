#pragma once
#include "Engine/GameObject.h"
class Player:public GameObject
{
	int hPlayerModel_;

public:
	//コンストラクタ
	Player(GameObject* parent);

	//デストラクタ
	~Player();

	//初期化
	void Initialize()override;

	//更新
	void Update()override;

	//描画
	void Draw()override;

	//関数
	void Release()override;

	XMFLOAT3  playerPosition;
};

