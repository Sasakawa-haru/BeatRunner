#pragma once
#include "Engine/GameObject.h"
#include"GameConfig.h"


class PlayScene : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	PlayScene(GameObject* parent);
	
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void SaveResultData();

private:
	GameConfig gameConfig_;
	enum class ClearState
	{
		Playing,
		PlayerPerformance,
		ShowClear,
		ChangeResult
	};
	ClearState clearState_ = ClearState::Playing;
	float clearTimer_ = 0.0f;

	int hClearImage_ = -1;
	Transform clearImageTransform_;
	float clearShowTime_ = 1.2f;
};