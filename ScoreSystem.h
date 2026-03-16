#pragma once
#include "Engine/GameObject.h"
#include "Engine/Text.h"
#include <string>

class ScoreSystem : public GameObject
{
public:
	enum class JudgeResult
	{
		Perfect,
		Great,
		Good,
		Normal,
		Miss
	};

	ScoreSystem(GameObject* parent);
	~ScoreSystem();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	void OnHit(int groupId, double diffSec);
	void OnCollision(int groupId);
	void OnJudge(JudgeResult r);

	void OnNormalPass();      // å©ì¶Çµóp
	void OnMissCollision();   // è’ìÀópMISS

private:
	void AddScore(int base);

	Text text_;

	int score_ = 0;
	int combo_ = 0;
	int maxCombo_ = 0;

	int perfect_ = 0;
	int great_ = 0;
	int good_ = 0;
	int normal_ = 0;
	int miss_ = 0;

	int judgeTimer_ = 0;
	std::string judgeText_;

	double lastDiffSec_ = 0.0;
	bool hasLastDiff_ = false;
};