#include "JudgeSystem.h"
#include "Music.h"
#include "ScoreSystem.h"
#include"OptionData.h"
#include "Engine/Input.h"
#include "Notes.h"
#include "NoteBase.h"
#include "RhythmConfig.h"
#include"RhythmLayout.h"
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

	if (!music || !music->IsStarted() || !notes || !score)
	{
		return;
	}

	const double now = music->GetNowSec() + gOptionData.JudgeTiming;

	for (int lane = 0; lane < kLaneCount_; ++lane)
	{
		if (Input::IsKeyDown(kLaneKey[lane]))
		{
			TryHitLane(lane, now, notes, score);
		}
	}

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

		if (!note) continue;
		if (note->IsDead()) continue;
		if (note->GetLane() != lane) continue;

		const double diff = nowSec - note->GetHitTimeSec();
		const double ad = std::abs(diff);

		if (ad > kNormal_) continue;

		if (ad < bestAbs)
		{
			bestAbs = ad;
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

		if (!note) continue;
		if (note->IsDead()) continue;

		if (note->GetPosition().z <= RhythmLayout::PassZ)
		{
			score->OnDodgeSuccess();
			note->KillMe();
		}
	}
}

ScoreSystem::JudgeResult JudgeSystem::CalcJudge(double diffSec) const
{
	const double ad = std::abs(diffSec);

	if (ad <= kPerfect_) return ScoreSystem::JudgeResult::Perfect;
	if (ad <= kGreat_) return ScoreSystem::JudgeResult::Great;
	if (ad <= kGood_) return ScoreSystem::JudgeResult::Good;
	if (ad <= kNormal_) return ScoreSystem::JudgeResult::Normal;

	return ScoreSystem::JudgeResult::Miss;
}