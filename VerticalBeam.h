#pragma once
#include "Engine/GameObject.h"
class VerticalBeam :public GameObject
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

private:
	int hVerticalBeamModel_;
};

