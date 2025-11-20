#include "Player.h"
#include"Stage.h"
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
	playerPosition = XMFLOAT3(0.0f, 1.0f, 0.0f);
	hPlayerModel_ = Model::Load("Models/Player.fbx");
	assert(hPlayerModel_ >= 0);
	SphereCollider* collision = new SphereCollider(playerPosition, 1.0f);
	AddCollider(collision);
}

void Player::Update()
{
	Stage* stage = (Stage*)FindObject("Stage");
	int hStageModel = stage->GetModelHandle();
	transform_.position_=playerPosition;

	RayCastData rayData;
	

}

void Player::Draw()
{
	Model::SetTransform(hPlayerModel_, transform_);
	Model::Draw(hPlayerModel_);
}

void Player::Release()
{
}
