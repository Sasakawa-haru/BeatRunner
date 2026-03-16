#include "JudgeSystem.h"
#include "Music.h"
#include "ScoreSystem.h"
#include "Engine/Input.h"
#include "Notes.h"
#include "NoteBase.h"
#include <cmath>

namespace {
	constexpr int kLaneKey[5] = { DIK_D, DIK_F, DIK_SPACE, DIK_J, DIK_K };
}

JudgeSystem::JudgeSystem(GameObject* parent)
	: GameObject(parent, "JudgeSystem")
{
}

JudgeSystem::~JudgeSystem() {}

void JudgeSystem::Initialize() {}
void JudgeSystem::Draw() {}
void JudgeSystem::Release() {}

void JudgeSystem::Update()
{
	auto* music = (Music*)FindObject("Music");
	auto* notes = (Notes*)FindObject("Notes");
	auto* score = (ScoreSystem*)FindObject("ScoreSystem");

	if (!music || !music->IsStarted() || !notes || !score) return;

	const double now = music->GetNowSec() + judgeOffsetSec_;
	for (int lane = 0; lane < kLaneCount_; ++lane)
	{
		if (Input::IsKeyDown(kLaneKey[lane]))
		{
			TryHitLane(lane, now, notes, score);
		}
	}

	// Œ©“¦‚µ‚Í Through ˆµ‚¢
	UpdateAutoNormal(now, notes, score);
}

void JudgeSystem::TryHitLane(int lane, double nowSec, Notes* notes, ScoreSystem* score)
{
	NoteBase* best = nullptr;
	double bestAbs = 1e18;

	for (auto* obj : *notes->GetChildList())
	{
		auto* note = dynamic_cast<NoteBase*>(obj);
		if (!note) continue;
		if (note->GetLane() != lane) continue;

		const double diff = nowSec - (double)note->GetHitTimeSec();
		const double ad = std::abs(diff);

		if (ad > kNormal_) continue;

		if (ad < bestAbs)
		{
			bestAbs = ad;
			best = note;
		}
	}

	if (!best) return;

	score->OnHit(best->GetGroupId(), nowSec - (double)best->GetHitTimeSec());
	best->KillMe();
}

void JudgeSystem::UpdateAutoNormal(double nowSec, Notes* notes, ScoreSystem* score)
{
	for (auto* obj : *notes->GetChildList())
	{
		auto* note = dynamic_cast<NoteBase*>(obj);
		if (!note) continue;
		if (note->IsDead()) continue;

		const double diff = nowSec - (double)note->GetHitTimeSec();

		// ”»’èŽž‚ð‰ß‚¬‚½ƒm[ƒc‚ÍŒ©“¦‚µNORMAL
		if (diff > kNormal_)
		{
			score->OnNormalPass();
			note->KillMe();
		}
	}
}

ScoreSystem::JudgeResult JudgeSystem::CalcJudge(double diffSec) const
{
	const double ad = std::abs(diffSec);
	if (ad <= kPerfect_) return ScoreSystem::JudgeResult::Perfect;
	if (ad <= kGreat_)   return ScoreSystem::JudgeResult::Great;
	if (ad <= kGood_)    return ScoreSystem::JudgeResult::Good;
	if (ad <= kNormal_)  return ScoreSystem::JudgeResult::Normal;
	return ScoreSystem::JudgeResult::Miss;
}