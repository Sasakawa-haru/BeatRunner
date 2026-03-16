#include "JudgeSystem.h"
#include "Music.h"
#include "ScoreSystem.h"
#include "Engine/Input.h"
#include "Notes.h"
#include "NoteBase.h"
#include "RhythmConfig.h"
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
	auto* player = FindObject("Player");

	if (!music || !music->IsStarted() || !notes || !score || !player) return;

	const double now = music->GetNowSec() + RhythmConfig::kJudgeOffsetSec;
	for (int lane = 0; lane < kLaneCount_; ++lane)
	{
		if (Input::IsKeyDown(kLaneKey[lane]))
		{
			TryHitLane(lane, now, notes, score);
		}
	}

	// 見逃しNORMALは、プレイヤーを通り過ぎてから
	UpdateAutoNormal(notes, score, player);
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

void JudgeSystem::UpdateAutoNormal(Notes* notes, ScoreSystem* score, GameObject* player)
{
	const float playerZ = player->GetPosition().z;

	for (auto* obj : *notes->GetChildList())
	{
		auto* note = dynamic_cast<NoteBase*>(obj);
		if (!note) continue;
		if (note->IsDead()) continue;

		// プレイヤー位置を十分通り過ぎたら見逃しNORMAL
		if (note->GetPosition().z < playerZ - 1.0f)
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