#include "JudgeSystem.h"

#include "Music.h"
#include "Notes.h"
#include "NoteBase.h"
#include "Player.h"
#include "ScoreSystem.h"
#include "OptionData.h"
#include "RhythmLayout.h"

#include <cmath>

JudgeSystem::JudgeSystem(GameObject* parent)
	: GameObject(parent, "JudgeSystem")
{
}

JudgeSystem::~JudgeSystem()
{
}

void JudgeSystem::Initialize()
{
	Enter();
	Visible();
}

void JudgeSystem::Update()
{
	Music* music = (Music*)FindObject("Music");
	Notes* notes = (Notes*)FindObject("Notes");
	ScoreSystem* score = (ScoreSystem*)FindObject("ScoreSystem");
	Player* player = (Player*)FindObject("Player");

	if (!music || !music->IsStarted() || !notes || !score || !player)
	{
		return;
	}

	const double nowSec = music->GetNowSec() + gOptionData.JudgeTiming;

	// Player操作があった瞬間だけ、音ゲー判定する
	if (player->IsRhythmActionTriggered())
	{
		int lane = player->GetLaneIndex();

		if (lane >= 0 && lane < 5)
		{
			TryHitLane(lane, nowSec, notes, score);
		}
	}

	// 音ゲー判定できずにプレイヤーを通り過ぎたノーツはDODGE扱い
	UpdateDodgeSuccess(notes, score);
}

void JudgeSystem::Draw()
{
}

void JudgeSystem::Release()
{
}

void JudgeSystem::TryHitLane(int lane, double nowSec, Notes* notes, ScoreSystem* score)
{
	NoteBase* best = nullptr;
	double bestAbs = 99999.0;

	for (auto* obj : *notes->GetChildList())
	{
		NoteBase* note = dynamic_cast<NoteBase*>(obj);

		if (!note)
		{
			continue;
		}

		if (note->IsDead())
		{
			continue;
		}

		if (note->GetLane() != lane)
		{
			continue;
		}

		const double diffSec = nowSec - note->GetHitTimeSec();
		const double absDiff = std::abs(diffSec);

		if (absDiff > kNormal_)
		{
			continue;
		}

		if (absDiff < bestAbs)
		{
			bestAbs = absDiff;
			best = note;
		}
	}

	if (!best)
	{
		return;
	}

	score->OnHit(best->GetGroupId(), nowSec - best->GetHitTimeSec());
	best->KillMe();
}

void JudgeSystem::UpdateDodgeSuccess(Notes* notes, ScoreSystem* score)
{
	for (auto* obj : *notes->GetChildList())
	{
		NoteBase* note = dynamic_cast<NoteBase*>(obj);

		if (!note)
		{
			continue;
		}

		if (note->IsDead())
		{
			continue;
		}

		if (note->GetPosition().z <= RhythmLayout::PassZ)
		{
			score->OnDodgeSuccess();
			note->KillMe();
		}
	}
}

ScoreSystem::JudgeResult JudgeSystem::CalcJudge(double diffSec) const
{
	const double absDiff = std::abs(diffSec);

	if (absDiff <= kPerfect_)
	{
		return ScoreSystem::JudgeResult::Perfect;
	}

	if (absDiff <= kGreat_)
	{
		return ScoreSystem::JudgeResult::Great;
	}

	if (absDiff <= kGood_)
	{
		return ScoreSystem::JudgeResult::Good;
	}

	if (absDiff <= kNormal_)
	{
		return ScoreSystem::JudgeResult::Normal;
	}

	return ScoreSystem::JudgeResult::Miss;
}