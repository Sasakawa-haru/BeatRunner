#include "VerticalBeam.h"
#include"Engine/Model.h"

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

}

void VerticalBeam::Update()
{
	//Model::SetTransform(hVerticalBeamModel_,)
}

void VerticalBeam::Draw()
{
}

void VerticalBeam::Release()
{
}
