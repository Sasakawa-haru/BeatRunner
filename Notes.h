#pragma once
#include"Engine/GameObject.h"
#include"BeatmapJsonLoader.h"
#include<memory>

class GameCsvReader;

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

	BeatMap map_;
	size_t nextIndex_ = 0;
	double nowSec_ = 0.0;

private:
	std::unique_ptr<GameCsvReader> notesCsv_;
	int nextLine_ = 1;
	int laneCount_ = 0;
	double nowSec_ = 0.0;
};

