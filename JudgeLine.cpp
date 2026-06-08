#include "JudgeLine.h"

#include "RhythmLayout.h"
#include "Lane.h"
#include "Engine/Model.h"

#include <cassert>

JudgeLine::JudgeLine(GameObject* parent)
	: GameObject(parent, "JudgeLine")
	, hJudgeLine_(-1)
{
}

JudgeLine::~JudgeLine()
{
}

void JudgeLine::Initialize()
{
	Lane* leftLane = Lane::FindByName("lane1");
	Lane* rightLane = Lane::FindByName("lane5");

	float centerX = 0.0f;
	float centerY = 0.0f;

	if (leftLane && rightLane)
	{
		centerX = (leftLane->GetLaneCenterX() + rightLane->GetLaneCenterX()) * 0.5f;

		XMFLOAT3 leftPos = leftLane->GetCenterPosition();
		centerY = leftPos.y + 0.02f;
	}

	transform_.position_ = XMFLOAT3(
		centerX,
		centerY,
		RhythmLayout::JudgeLineZ
	);

	transform_.scale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);

	hJudgeLine_ = Model::Load("Models/JudgeLine.fbx");
	assert(hJudgeLine_ >= 0);

	Enter();
	Visible();
}

void JudgeLine::Update()
{
}

void JudgeLine::Draw()
{
	if (hJudgeLine_ < 0)
	{
		return;
	}

	Model::SetTransform(hJudgeLine_, transform_);
	Model::Draw(hJudgeLine_);
}

void JudgeLine::Release()
{
}