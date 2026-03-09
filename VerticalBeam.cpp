#include "VerticalBeam.h"
#include"Lane.h"
#include"Engine/Model.h"
#include"Engine/Time.h"

#include<cassert>

namespace
{
	constexpr float kBeamSpeed = 20.0f;
	constexpr float kKillz = -10.0f;
}

VerticalBeam::VerticalBeam(GameObject* parent)
	:NoteBase(parent,"VerticalBeam"),hVerticalBeamModel_(-1)
{
}

VerticalBeam::~VerticalBeam()
{
}

void VerticalBeam::Initialize()
{
	hVerticalBeamModel_ = Model::Load("Models/VerticalBeam.fbx");
	Enter();\
	Visible();
}

void VerticalBeam::Update()
{
	float dt =Time::DeltaTime();
	transform_.position_.z -= kBeamSpeed * dt;

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
