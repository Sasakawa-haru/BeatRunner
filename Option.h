#pragma once
#include"Engine/GameObject.h"
#include<string>

enum OPTION
{
	NOTES_SPEED,
	JUDGE_TIMING,
	MASTER_VOLUME,
	MUSIC_VOLUME,
	SE_VOLUME
};

class Option:GameObject
{
public:
	//コンストラクタ
	Option(GameObject* parent);
	//デストラクタ
	~Option();
	//初期化
	void Initialize() override;
	//更新
	void Update()override;
	//描画
	void Draw()override;
	//解放
	void Release()override;

	//プレイシーン関連
	float NotesSpeed=5.0f;
	float JudgeTiming=0.0f;

	//音量関連（0.0f~1.0f）
	float MasterVolume = 1.0f;
	float MusicVolume=1.0f;
	float SeVolume=1.0f;

private:
	OPTION option_;

};

