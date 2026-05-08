#pragma once
#include"Engine/GameObject.h"
#include<string>
class Result:GameObject
{
public:
	//コンストラクタ
	Result(GameObject* parent);
	//デストラクタ
	~Result();
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw()override;
	//解放
	void Release()override;

private:
	
};

