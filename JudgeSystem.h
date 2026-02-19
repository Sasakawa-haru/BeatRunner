#pragma once
#include "Engine/GameObject.h"

class Notes;
class Music;
class ScoreSystem;

class NoteBase;

class JudgeSystem:public GameObject
{
public:
	JudgeSystem(GameObject* parent);
	~JudgeSystem()override;

	void Initialize()override;
	void Update()override;
	void Draw() override;
	void Release()override;

private:
	void TryHitLane(int lane, double nowSec, Notes* notes, ScoreSystem* score);
	
	static constexpr double kBadWindow_ = 0.150;
	static constexpr int kLaneCount_ = 5;
};

