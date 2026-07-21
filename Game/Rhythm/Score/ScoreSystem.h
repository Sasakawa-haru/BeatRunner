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

	void OnNormalPass();      // 見逃し用
	void OnDodgeSuccess();   // 回避成功用
	void OnMissCollision();   // 衝突用MISS
	void SetScoreMaxCombo(int maxCombo);// 最大コンボ数を設定（スコア計算用）
	void FixPerfectScoreIfNeeded();//全判定がパーフェクトだった場合のスコア補正

	int GetScore()const { return score_; }
	int GetMaxCombo()const { return maxCombo_; }

	int GetPerfect()const { return perfect_; }
	int GetGreat()const { return great_; }
	int GetGood()const { return good_; }
	int GetNormal()const { return normal_; }
	int GetMiss()const { return miss_; }


private:
	void AddScore(double rate);

	Text text_;

	int score_ = 0;
	int combo_ = 0;
	int maxCombo_ = 0;

	static constexpr int kMaxScore = 1000000;

	int scoreMaxCombo_ = 0;
	double scoreRaw_ = 0.0;
	int perfect_ = 0;
	int great_ = 0;
	int good_ = 0;
	int normal_ = 0;
	int miss_ = 0;

	int judgeTimer_ = 0;
	std::string judgeText_;

	double lastDiffSec_ = 0.0;
	bool hasLastDiff_ = false;

	int hMissImage_ = -1;
	int missEffectTimer_ = 0;
	Transform missEffectTransform_;
};