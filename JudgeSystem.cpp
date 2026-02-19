#include "JudgeSystem.h"
#include"Music.h"
#include"ScoreSystem.h"
#include"Engine/Input.h"
#include"Notes.h"
#include"NoteBase.h"
#include<cmath>

namespace {
	constexpr int kLaneKey[5] = { DIK_D,DIK_F,DIK_SPACE,DIK_J,DIK_K };
}

void JudgeSystem::Update()
{
	auto* music = (Music*)FindObject("Music");
	auto* notes = (Notes*)FindObject("Notes");
	auto* score = (ScoreSystem*)FindObject("ScoreSystem");

	if (!music || !music->IsStarted() || !notes || !score)return;
	const double now = music->GetNowSec();

	for (int lane = 0; lane < kLaneCount_; ++lane) {
		if (Input::IsKeyDown(kLaneKey[lane])) {
			TryHitLane(lane, now, notes, score);
		}
	}
}

JudgeSystem::JudgeSystem(GameObject* parent)
	: GameObject(parent, "JudgeSystem")
{
}

JudgeSystem::~JudgeSystem() {}

void JudgeSystem::Initialize() {}
void JudgeSystem::Draw() {}
void JudgeSystem::Release() {}

void JudgeSystem::TryHitLane(int lane, double nowSec, Notes* notes, ScoreSystem* score)
{
	NoteBase* best = nullptr;
	double bestAbs = 1e18;

	for (auto* obj : *notes->GetChildList()) {
		auto* note = dynamic_cast<NoteBase*>(obj);
		if (!note)continue;
		if (note->GetLane() != lane)continue;
		const double diff = nowSec - (double)note->GetHitTimeSec();
		const double ad = std::abs(diff);

		if (ad > kBadWindow_)continue;
		if (ad < bestAbs) {
			bestAbs = ad;
			best = note;
		}
	}

	if (!best)return;

	score->OnHit(best->GetGroupId(), nowSec - (double)best->GetHitTimeSec());
	best->KillMe();
}
