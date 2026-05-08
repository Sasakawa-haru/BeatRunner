#pragma once
#include"Engine/GameObject.h"
#include<string>
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

private:
	float NotesSpeed=0;
	float JudgeTiming=0;
	float MusicVolume=0;
	float SeVolume=0;
};

