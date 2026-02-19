#pragma once
#include"Engine/GameObject.h"
#include"Engine/Text.h"
class ScoreSystem :public GameObject
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

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void Release()override;

	void OnJudge(JudgeResult r);

private:
	void AddScore(int base);

	Text text_;

	int score_ = 0;
	int combo_ = 0;
	int maxCombo_ = 0;

	int perfect_;
	int great_;
	int good_;
	int normal_;
	int miss_;
	

};

