#pragma once
#include"Engine/GameObject.h"

class JudgeLine:public GameObject
{
public:
	JudgeLine(GameObject* parent);
	~JudgeLine()override;

	void Initialize()override;
	void Update() override;
	void Draw()override;
	void Release();
private:
	int hJudgeLine_;
};

