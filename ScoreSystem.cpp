#include "ScoreSystem.h"
#include"Engine/Camera.h"
#include <algorithm>
#include <cmath>

ScoreSystem::ScoreSystem(GameObject* parent)
	: GameObject(parent, "ScoreSystem")
{
}

ScoreSystem::~ScoreSystem()
{
}

void ScoreSystem::Initialize()
{
	hMissImage_ = Image::Load("Effect.png");
	assert(hMissImage_ >= 0);

	missEffectTimer_ = 0;
	missEffectTransform_ = Transform();
	missEffectTransform_.position_ = XMFLOAT3(0.0f, 0.02f, -0.02f);   
	missEffectTransform_.scale_ = XMFLOAT3(0.25f, 0.25f, 1.0f);
	text_.Initialize();

	score_ = 0;
	combo_ = 0;
	maxCombo_ = 0;

	perfect_ = 0;
	great_ = 0;
	good_ = 0;
	normal_ = 0;
	miss_ = 0;

	judgeTimer_ = 0;
	judgeText_.clear();

	Enter();
	Visible();
}

void ScoreSystem::Update()
{
	if (judgeTimer_ > 0)
	{
		judgeTimer_--;
	}

	if (missEffectTimer_ > 0)
	{
		missEffectTimer_--;
	}
}

void ScoreSystem::Draw()
{
	text_.Draw(20, 20, "SCORE:");
	text_.Draw(140, 20, score_);

	text_.Draw(20, 60, "COMBO:");
	text_.Draw(140, 60, combo_);

	if (judgeTimer_ > 0)
	{
		text_.Draw(20, 100, judgeText_.c_str());
	}
	if (hasLastDiff_)
	{
		int diffMs = (int)(lastDiffSec_ * 1000.0);

		text_.Draw(20, 140, "DIFF(ms):");
		text_.Draw(180, 140, diffMs);
	}

	if (missEffectTimer_ > 0)
	{
		Image::SetTransform(hMissImage_, missEffectTransform_);
		Image::Draw(hMissImage_);
	}
}

void ScoreSystem::Release()
{
}

void ScoreSystem::OnHit(int groupId, double diffSec)
{
	lastDiffSec_ = diffSec;
	hasLastDiff_ = true;
	const double ad = std::abs(diffSec);

	constexpr double kPerfect = 0.035;
	constexpr double kGreat = 0.070;
	constexpr double kGood = 0.110;
	constexpr double kNormal = 0.160;

	if (ad <= kPerfect) { OnJudge(JudgeResult::Perfect); return; }
	if (ad <= kGreat) { OnJudge(JudgeResult::Great);   return; }
	if (ad <= kGood) { OnJudge(JudgeResult::Good);    return; }
	if (ad <= kNormal) { OnJudge(JudgeResult::Normal);  return; }

}

void ScoreSystem::OnCollision(int groupId)
{
}

void ScoreSystem::OnJudge(JudgeResult r)
{
	switch (r)
	{
	case JudgeResult::Perfect:
		perfect_++;
		combo_++;
		maxCombo_ = (std::max)(maxCombo_, combo_);
		AddScore(1000);
		judgeText_ = "PERFECT";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Great:
		great_++;
		combo_++;
		maxCombo_ = (std::max)(maxCombo_, combo_);
		AddScore(700);
		judgeText_ = "GREAT";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Good:
		good_++;
		combo_++;
		maxCombo_ = (std::max)(maxCombo_, combo_);
		AddScore(500);
		judgeText_ = "GOOD";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Normal:
		normal_++;
		combo_ = 0;
		maxCombo_ = (std::max)(maxCombo_, combo_);
		AddScore(100);
		judgeText_ = "NORMAL";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Miss:
		miss_++;
		combo_ = 0;
		maxCombo_ = (std::max)(maxCombo_, combo_);
		AddScore(0);
		judgeText_ = "MISS";
		judgeTimer_ = 20;
		break;
	}
}

void ScoreSystem::OnNormalPass()
{
	normal_++;
	combo_ = 0;
	maxCombo_ = (std::max)(maxCombo_, combo_);

	AddScore(10);

	//judgeText_ = "Through";
	judgeTimer_ = 20;
}

void ScoreSystem::OnMissCollision()
{
	miss_++;
	combo_ = 0;
	maxCombo_ = (std::max)(maxCombo_, combo_);
	AddScore(0);

	judgeText_ = "MISS";
	judgeTimer_ = 20;
	missEffectTimer_ = 20;

	Camera::StartShake(0.25f, 0.25f);
}

void ScoreSystem::AddScore(int base)
{
	float comboMul = 1.0f + (combo_ * 0.01f);
	score_ += (int)(base * comboMul);

}