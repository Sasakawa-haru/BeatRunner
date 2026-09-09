#pragma once

#include "Engine/GameObject.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

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

    const std::vector<double>& GetGroupTimes() const
    {
        return groupTimes_;
    }

    int GetGroupId(double hitTime) const
    {
        return timeToGroup_.at(hitTime);
    }

    const std::vector<int>& GetGroupTimesMs() const
    {
        return groupTimesMs_;
    }

    int GetGroupIdByTimeMs(int tms) const;
    int GetAllNotesCount() const;

    // 現在、次に判定されるグループID
    int GetNearestGroupId() const
    {
        return nearestGroupId_;
    }

private:
    void BuildGroupsFromCsv();
    void UpdateNearestGroup();

private:
    std::vector<int> groupTimesMs_;
    std::unordered_map<int, int> timeMsToGroupId_;

    std::unique_ptr<GameCsvReader> notesCsv_;

    std::vector<double> groupTimes_;
    std::unordered_map<double, int> timeToGroup_;

    std::string musicLevel;

    int nextLine_ = 1;
    int laneCount_ = 0;

    double nowSec_ = 0.0;

    int comboCount = 0;
    int maxCombo = 0;

    static constexpr double NOTE_START_ADVANCE_SEC = 1.0;

    // 次に来るノーツグループ
    int nearestGroupId_ = -1;
};