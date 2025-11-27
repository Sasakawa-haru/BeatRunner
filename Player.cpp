#include "Player.h"
#include"Stage.h"
#include"Lane.h"
#include"Engine/Model.h"
#include"Engine/Input.h"
#include"Engine/SphereCollider.h"



Player::Player(GameObject* parent)
	:GameObject(parent,"Player"),hPlayerModel_(-1)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	Lane* centerLane = nullptr;
	transform_.position_= XMFLOAT3(0.0f, 3.0f, 0.0f);
	playerPosition = XMFLOAT3(0.0f, 3.0f, 0.0f);
	hPlayerModel_ = Model::Load("Models/Player.fbx");
	assert(hPlayerModel_ >= 0);
	SphereCollider* collision = new SphereCollider(playerPosition, 3.0f);
	AddCollider(collision);
}

void Player::Update()
{
	Stage* stage = (Stage*)FindObject("Stage");
	int hStageModel = stage->GetModelHandle();

	Lane* lane = (Lane*)FindObject("Lane");
	int hLaneModel = lane->GetLaneHandle();

	RayCastData rayData;
	rayData.start = transform_.position_;
	rayData.start.y = 0;
	rayData.dir = XMFLOAT3(0, -1, 0);

	Model::RayCast(hLaneModel, &rayData);
	if (rayData.hit)
	{
		transform_.position_.y = -rayData.dist;
	}

	if(Input::IsKeyDown(DIK_A))
	{
		transform_.position_.x-=2.0f;
	}
	if (Input::IsKeyDown(DIK_D))
	{
		transform_.position_.x += 2.0f;
	}
	if (Input::IsKeyDown(DIK_SPACE)&&!isJumping_)
	{
		isJumping_ = true;
		jumpVelocity_ = jumpV0;
	}
	if (!isJumping_ && Input::IsKeyDown(DIK_SPACE)) {
		isJumping_ = true;
		jumpVelocity_ = jumpV0;
	}

	// ジャンプ中の処理
	if (isJumping_) {
		jumpVelocity_ += gravity_;
		transform_.position_.y += jumpVelocity_ * 0.1f; // 0.1fはフレーム調整用

		// 地面判定（raycastで地面に着いたらジャンプ終了）
		RayCastData rayData;
		rayData.start = transform_.position_;
		rayData.start.y = 0;
		rayData.dir = XMFLOAT3(0, -1, 0);

		Model::RayCast(hLaneModel, &rayData);
		if (rayData.hit && transform_.position_.y <= -rayData.dist) {
			transform_.position_.y = -rayData.dist;
			isJumping_ = false;
			jumpVelocity_ = 0.0f;
		}
	}
	else {
		// 地面にいるときのY座標補正
		RayCastData rayData;
		rayData.start = transform_.position_;
		rayData.start.y = 0;
		rayData.dir = XMFLOAT3(0, -1, 0);

		Model::RayCast(hLaneModel, &rayData);
		if (rayData.hit) {
			transform_.position_.y = -rayData.dist;
		}
	}
}

void Player::Draw()
{
	Model::SetTransform(hPlayerModel_, transform_);
	Model::Draw(hPlayerModel_);
}

void Player::Release()
{
}
