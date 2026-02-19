#include "ScoreSystem.h"
#include<algorithm>


ScoreSystem::ScoreSystem(GameObject* parent)
	:GameObject(parent,"ScoreSystem")
{
}

ScoreSystem::~ScoreSystem()
{
}

void ScoreSystem::Initialize()
{
	text_.Initialize();

	score_ = 0;
	combo_ = 0;
	maxCombo_ = 0;

	perfect_ = great_ = good_ = normal_ = miss_ = 0;
	Enter();
	Visible();
}

void ScoreSystem::Update()
{
}

void ScoreSystem::Draw()
{
	text_.Draw(20, 20, "SCORE:");
	text_.Draw(140, 20, score_);
}

void ScoreSystem::Release()
{
}

void ScoreSystem::OnJudge(JudgeResult r)
{
	switch (r)
	{
	case JudgeResult::Perfect:
		perfect_++;
		combo_++;
		maxCombo_ = ((std::max)(maxCombo_, combo_));
		AddScore(1000);
		break;
	case JudgeResult::Great:
		great_++;
		combo_++;
		maxCombo_ = ((std::max)(maxCombo_, combo_));
		AddScore(1000);
		break;
	case JudgeResult::Good:
		good_++;
		combo_++;
		maxCombo_ = ((std::max)(maxCombo_, combo_));
		AddScore(1000);
		break;
	case JudgeResult::Normal:
		normal_++;
		combo_ = 0;
		maxCombo_ = ((std::max)(maxCombo_, combo_));
		AddScore(1000);
		break;
	case JudgeResult::Miss:
		miss_++;
		combo_ = 0;
		maxCombo_ = ((std::max)(maxCombo_, combo_));
		AddScore(1000);
		break;


	}

}

void ScoreSystem::AddScore(int base)
{
	float comboMul = 1.0f + (combo_ * 0.01f);
	score_ += (int)(base * comboMul);

}
