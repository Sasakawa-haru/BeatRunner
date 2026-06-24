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
	judgedGroups_.clear();
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

    // まず、押したレーンで一番タイミングが近いノーツを探す
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

        const int groupId = note->GetGroupId();
        if (IsGroupJudged(groupId))
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

    const int targetGroupId = best->GetGroupId();

    // 同じグループ内で一番タイミングが良いdiffを探す
    double bestGroupDiff = nowSec - best->GetHitTimeSec();
    double bestGroupAbs = std::abs(bestGroupDiff);

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

        if (note->GetGroupId() != targetGroupId)
        {
            continue;
        }

        const double diffSec = nowSec - note->GetHitTimeSec();
        const double absDiff = std::abs(diffSec);

        if (absDiff < bestGroupAbs)
        {
            bestGroupAbs = absDiff;
            bestGroupDiff = diffSec;
        }
    }

    score->OnHit(targetGroupId, bestGroupDiff);

    judgedGroups_.insert(targetGroupId);

    KillGroupNotes(notes, targetGroupId);
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

        const int groupId = note->GetGroupId();

        if (IsGroupJudged(groupId))
        {
            continue;
        }

        if (note->GetPosition().z <= RhythmLayout::PassZ)
        {
            score->OnDodgeSuccess();

            judgedGroups_.insert(groupId);
            KillGroupNotes(notes, groupId);
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

void JudgeSystem::KillGroupNotes(Notes* notes, int groupId)
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

        if (note->GetGroupId() == groupId)
        {
            note->KillMe();
        }
    }
}

bool JudgeSystem::IsGroupJudged(int groupId)const
{
    if (groupId < 0)
    {
        return false;
    }
    return judgedGroups_.find(groupId) != judgedGroups_.end();
}