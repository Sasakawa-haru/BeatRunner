#include "BesideBeam.h"
#include"Lane.h"
#include"Engine/Model.h"
#include"Engine/Time.h"

BesideBeam::BesideBeam(GameObject* parent)
	:GameObject(parent,"BesideBeam"),  hBesideBeamModel_(-1)
{
	
}

BesideBeam::~BesideBeam()
{
}

void BesideBeam::Initialize()
{
	hBesideBeamModel_ = Model::Load("Models/BesideBeam.fbx");
	Enter();
	Visible();
	Lane* lane = (Lane*)FindObject("Lane");
	transform_.position_.x += Lane::laneWidth / 2;
	assert(lane);


}

void BesideBeam::Update()
{
	const float speed = 20.0f;
	float dt = Time::DeltaTime();
	transform_.position_.z -= speed * dt;

	if (transform_.position_.z < -10.0f) {
		KillMe();
	}
}

void BesideBeam::Draw()
{
	Model::SetTransform(hBesideBeamModel_, transform_);
	Model::Draw(hBesideBeamModel_);
}

void BesideBeam::Release()
{
}
