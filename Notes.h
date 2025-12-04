#pragma once
#include"Engine/GameObject.h"

class Notes:public GameObject
{
public:
	//コンストラクタ
	Notes(GameObject* parent);
	
	//デストラクタ
	~Notes();
	
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

