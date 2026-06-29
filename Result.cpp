#include "Result.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"
#include"Engine/Text.h"

#include"ResultData.h"

#include<string>

namespace
{
	constexpr int kTitleX = 100;
	constexpr int kTitleY = 80;

	constexpr int kScoreX = 100;
	constexpr int kScoreY = 140;

	constexpr int kMaxComboX = 100;
	constexpr int kMaxComboY = 180;

	constexpr int kPerfectX = 100;
	constexpr int kPerfectY = 240;

	constexpr int kGreatX = 100;
	constexpr int kGreatY = 280;

	constexpr int kGoodX = 100;
	constexpr int kGoodY = 320;

	constexpr int kNormalX = 100;
	constexpr int kNormalY = 360;

	constexpr int kMissX = 100;
	constexpr int kMissY = 400;

	constexpr int kGuideX = 100;
	constexpr int kGuideY = 480;
}

Result::Result(GameObject* parent)
{
}

Result::~Result()
{
}

void Result::Initialize()
{
	text_.Initialize();
}

void Result::Update()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");

		if (pSceneManager)
		{
			pSceneManager->ChangeScene(SCENE_ID_SELECT);
		}
	}
}

void Result::Draw()
{
	text_.Draw(kTitleX, kTitleY, "Result");
	text_.Draw(kScoreX, kScoreY, ("Score:" + std::to_string(gResultData.score)).c_str());
	text_.Draw(kMaxComboX, kMaxComboY, ("Max Combo:" + std::to_string(gResultData.maxCombo)).c_str());
	text_.Draw(kPerfectX, kPerfectY, ("Perfect:" + std::to_string(gResultData.perfect)).c_str());
	text_.Draw(kGreatX, kGreatY, ("Great:" + std::to_string(gResultData.great)).c_str());
	text_.Draw(kGoodX, kGoodY, ("Good:" + std::to_string(gResultData.good)).c_str());
	text_.Draw(kNormalX, kNormalY, ("Normal:" + std::to_string(gResultData.normal)).c_str());
	text_.Draw(kMissX, kMissY, ("Miss:" + std::to_string(gResultData.great)).c_str());
}

void Result::Release()
{
}
