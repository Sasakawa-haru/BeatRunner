#include "VerticalBeam.h"
#include"Lane.h"
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
	Lane* lane = (Lane*)FindObject("Lane");
	assert(lane);

}

void VerticalBeam::Update()
{
	//Model::SetTransform(hVerticalBeamModel_,)
}

void VerticalBeam::Draw()
{
	Model::SetTransform(hVerticalBeamModel_, transform_);
	Model::Draw(hVerticalBeamModel_);
}

void VerticalBeam::Release()
{
}
