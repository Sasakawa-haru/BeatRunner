#pragma once
#include "Engine/GameObject.h"
#include"Engine/SphereCollider.h"
class Player:public GameObject
{

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
	int hPlayerModel_;
	float radius_=3.0f;

	bool isJumping_ = false;
	float jumpVelocity_= 0.0f;
	const float gravity = -0.3f;
	const float jumpV0 = 3.0f;
	float groundY_ = 0.0f;

	SphereCollider* collider_ = nullptr;
};

