#pragma once
#include"Engine/GameObject.h"

class ClearSene:public GameObject
{
public:
	ClearSene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};

