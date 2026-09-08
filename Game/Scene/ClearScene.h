#pragma once
#include"Engine/GameObject.h"
#include"Engine/Text.h"

class ClearScene:public GameObject
{
public:
	ClearScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	Text text_;
};

