#pragma once
#include "Engine/GameObject.h"
#include "ScoreSystem.h"

class Notes;
class Music;
class ScoreSystem;
class NoteBase;

class JudgeSystem : public GameObject
{
public:
	JudgeSystem(GameObject* parent);
	~JudgeSystem() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	void TryHitLane(int lane, double nowSec, Notes* notes, ScoreSystem* score);
	void UpdateAutoNormal(Notes* notes, ScoreSystem* score, GameObject* player);	ScoreSystem::JudgeResult CalcJudge(double diffSec) const;

	static constexpr int kLaneCount_ = 5;

	double judgeOffsetSec_ = 0.00;

	static constexpr double kPerfect_ = 0.030;
	static constexpr double kGreat_ = 0.060;
	static constexpr double kGood_ = 0.100;
	static constexpr double kNormal_ = 0.150;
};