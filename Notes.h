#pragma once

#include "Engine/GameObject.h"
#include<unordered_map>
#include <memory>
#include<string>

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

    const std::vector<double>& GetGroupTimes()const { return groupTimes_; }
    int GetGroupId(double hitTime)const { return timeToGroup_.at(hitTime); }
    const std::vector<int>& GetGroupTimesMs()const { return groupTimesMs_; }
    int GetGroupIdByTimeMs(int tms)const;
private:
    void BuildGroupsFromCsv();
    std::vector<int>groupTimesMs_;
    std::unordered_map<int, int>timeMsToGroupId_;

    std::unique_ptr<GameCsvReader> notesCsv_;
    std::vector<double>groupTimes_;
    std::unordered_map<double, int> timeToGroup_;
    std::string musicLevel;
    int nextLine_ = 1;    //csvヘッダー分の1
    int laneCount_ = 0;   // CSV列数-1
    double nowSec_ = 0.0; // 曲の経過秒
    int comboCount = 0;  //コンボ数
    int maxCombo;        //最大コンボ数

};
