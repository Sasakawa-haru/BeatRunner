#pragma once

#include "Engine/GameObject.h"
#include <memory>

class GameCsvReader;

class Notes : public GameObject
{
public:
    Notes(GameObject* parent);
    ~Notes();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

private:
    std::unique_ptr<GameCsvReader> notesCsv_;
    int nextLine_ = 1;    //csvヘッダー分の1
    int laneCount_ = 0;   // CSV列数-1
    double nowSec_ = 0.0; // 曲の経過秒（理想は音源再生位置）
    int comboCount = 0;  //コンボ数
    int maxCombo;        //最大コンボ数

};
