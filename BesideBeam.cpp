#include "BesideBeam.h"
#include"Engine/Model.h"
#include"Engine/Time.h"
#include <windows.h>

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

}

void BesideBeam::Update()
{
	static int c = 0;
	if ((c++ % 60) == 0) OutputDebugStringA("BesideBeam Update\n");
	const float speed = 10.0f;
	float dt = Time::DeltaTime();
	transform_.position_.z -= speed * dt;

	if (transform_.position_.z < -10.0f) {
		KillMe();
	}
	char buf[128];
	sprintf_s(buf, "dt=%.6f z=%.3f\n", Time::DeltaTime(), transform_.position_.z);
	OutputDebugStringA(buf);
}

void BesideBeam::Draw()
{
	Model::SetTransform(hBesideBeamModel_, transform_);
	Model::Draw(hBesideBeamModel_);
}

void BesideBeam::Release()
{
}
