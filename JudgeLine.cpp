#include "JudgeLine.h"
#include "RhythmLayout.h"
#include"Engine/Model.h"

JudgeLine::JudgeLine(GameObject* parent)
	:GameObject(parent,"JudgeLine")
	,hJudgeLine_(-1)
{
}

JudgeLine::~JudgeLine()
{
}

void JudgeLine::Initialize()
{
	transform_.position_ = XMFLOAT3(0.0f, 0.05f, RhythmLayout::JudgeLineZ);
	hJudgeLine_ = Model::Load("Models/JudgeLine.fbx");
	assert(hJudgeLine_ >= 0);

}

void JudgeLine::Update()
{
}

void JudgeLine::Draw()
{
	Model::SetTransform(hJudgeLine_, transform_);
	Model::Draw(hJudgeLine_);
}

void JudgeLine::Release()
{
}
