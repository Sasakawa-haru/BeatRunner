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
}

void Player::Draw()
{
	Model::SetTransform(hPlayerModel_, transform_);
	Model::Draw(hPlayerModel_);
}

void Player::Release()
{
}
