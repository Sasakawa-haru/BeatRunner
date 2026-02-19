#pragma once
#include "Engine/GameObject.h"
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
};

