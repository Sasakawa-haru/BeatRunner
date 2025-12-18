#include "VerticalBeam.h"
#include"Lane.h"
#include"Engine/Model.h"
#include"Engine/Time.h"

VerticalBeam::VerticalBeam(GameObject* parent)
	:GameObject(parent,"VertivalBeam"),hVerticalBeamModel_(-1)
{
}

VerticalBeam::~VerticalBeam()
{
}

void VerticalBeam::Initialize()
{
	hVerticalBeamModel_ = Model::Load("Models/VerticalBeam.fbx");
	Enter();
	Visible();
	Lane* lane = (Lane*)FindObject("Lane");
	assert(lane);

}

void VerticalBeam::Update()
{
	const float speed = 10.0f;
	float dt =Time::DeltaTime();
	transform_.position_.z -= speed * dt;

	if (transform_.position_.z < -10.0f) {
		KillMe();
	}
}

void VerticalBeam::Draw()
{
	Model::SetTransform(hVerticalBeamModel_, transform_);
	Model::Draw(hVerticalBeamModel_);
}

void VerticalBeam::Release()
{
}
