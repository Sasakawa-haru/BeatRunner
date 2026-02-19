#include "JudgeSystem.h"

struct Judge {
	int parfect;//完璧
	int great;//少しのズレ
	int good;//大幅なズレ
	int normal;//タイミング関係なしに素通りした場合
	int but;//当たった場合
};

JudgeSystem::JudgeSystem(GameObject* parent)
	: GameObject(parent, "JudgeSystem")
{
}

JudgeSystem::~JudgeSystem() {}

void JudgeSystem::Initialize() {}
void JudgeSystem::Update() {}
void JudgeSystem::Draw() {}
void JudgeSystem::Release() {}