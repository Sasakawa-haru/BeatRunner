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

private:
	bool isJumping_ = false;
	float jumpVelocity_ = 0.0f;
	const float gravity_ = -0.5f;
	const float jumpV0 = 8.0f;
};

