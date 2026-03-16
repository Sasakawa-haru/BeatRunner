#pragma once
#include"NoteBase.h"
#include "Engine/GameObject.h"
#include"Player.h"
class VerticalBeam :public NoteBase
{
public:
	//コンストラクタ
	VerticalBeam(GameObject* parent);

	//デストラクタ
	~VerticalBeam();

	//初期化
	void Initialize()override;

	//更新
	void Update()override;

	//描画
	void Draw()override;

	//関数
	void Release();

	void OnCollision(GameObject* pTarget)override;

private:
	int hVerticalBeamModel_;
	bool PlayerHit = false;
};

