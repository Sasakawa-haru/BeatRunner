#pragma once
#include"Engine/GameObject.h"
#include"Engine/Text.h"

class ClearSene:public GameObject
{
public:
	ClearSene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	Text text_;
};

