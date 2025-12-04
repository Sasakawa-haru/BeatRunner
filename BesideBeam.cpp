#include "BesideBeam.h"
#include"Engine/Model.h"

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
}

void BesideBeam::Draw()
{
	//Model::SetTransform(hBesideBeamModel_,)
}

void BesideBeam::Release()
{
}
