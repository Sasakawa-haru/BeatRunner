#include "Game/Rhythm/Score/ScoreSystem.h"
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
	// =========================
	// スコア
	// =========================
	text_.Draw(
		SCORE_LABEL_X,
		SCORE_Y,
		"SCORE:"
	);

	text_.Draw(
		SCORE_VALUE_X,
		SCORE_Y,
		score_
	);

	// =========================
	// コンボ
	// =========================
	text_.Draw(
		COMBO_LABEL_X,
		COMBO_Y,
		"COMBO:"
	);

	text_.Draw(
		COMBO_VALUE_X,
		COMBO_Y,
		combo_
	);

	// =========================
	// 判定表示
	// =========================
	if (judgeTimer_ > 0)
	{
		text_.Draw(
			JUDGE_TEXT_X,
			JUDGE_TEXT_Y,
			judgeText_.c_str()
		);
	}

	// =========================
	// タイミング差表示
	// =========================
	if (hasLastDiff_)
	{
		const int diffMs =
			static_cast<int>(
				lastDiffSec_ * SEC_TO_MILLISEC
				);

		text_.Draw(
			DIFF_LABEL_X,
			DIFF_Y,
			"DIFF(ms):"
		);

		text_.Draw(
			DIFF_VALUE_X,
			DIFF_Y,
			diffMs
		);
	}

	// =========================
	// MISS演出
	// =========================
	if (missEffectTimer_ > 0)
	{
		Image::SetTransform(
			hMissImage_,
			missEffectTransform_
		);

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

		AddScore(1.0);

		judgeText_ = "PERFECT";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Great:
		great_++;
		combo_++;
		maxCombo_ = (std::max)(maxCombo_, combo_);

		AddScore(0.7);

		judgeText_ = "GREAT";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Good:
		good_++;
		combo_++;
		maxCombo_ = (std::max)(maxCombo_, combo_);

		AddScore(0.5);

		judgeText_ = "GOOD";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Normal:
		normal_++;
		combo_ = 0;
		maxCombo_ = (std::max)(maxCombo_, combo_);

		AddScore(0.1);

		judgeText_ = "NORMAL";
		judgeTimer_ = 20;
		break;

	case JudgeResult::Miss:
		miss_++;
		combo_ = 0;
		maxCombo_ = (std::max)(maxCombo_, combo_);

		AddScore(0.0);

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

	AddScore(0.1);

	judgeTimer_ = 20;
}

void ScoreSystem::OnDodgeSuccess()
{
	normal_++;

	maxCombo_ = (std::max)(maxCombo_, combo_);

	AddScore(0.1);

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

void ScoreSystem::SetScoreMaxCombo(int maxCombo)
{
	if(maxCombo<=0)
	{ 
		scoreMaxCombo_ = 1;
		return;
	}
	scoreMaxCombo_ = maxCombo;
}

void ScoreSystem::AddScore(double rate)
{
	const double scorePerNote = static_cast<double>(kMaxScore) / static_cast<double>(scoreMaxCombo_);
	scoreRaw_ += scorePerNote * rate;
	if (scoreRaw_ > kMaxScore)
	{
		scoreRaw_ = kMaxScore;
	}
	score_ = static_cast<int>(scoreRaw_+0.5);
	FixPerfectScoreIfNeeded();
}
void ScoreSystem::FixPerfectScoreIfNeeded()
{
	if (perfect_ == scoreMaxCombo_)
	{
		scoreRaw_ = static_cast<double>(kMaxScore);
		score_ = kMaxScore;
	}
}