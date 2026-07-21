#pragma once

#include <string>

class GameConfig
{
public:
    bool LoadFromCsv(const std::string& filePath);

    int GetLaneCount() const { return m_laneCount; }
    float GetLaneWidth()const { return m_laneWidth; }

    float GetSpawnZ() const { return m_spawnZ; }
    float GetJudgeZ() const { return m_judgeZ; }
    float GetPlayerZ() const { return m_playerZ; }
    float GetPassZ() const { return m_passZ; }
    float GetNoteSpeed() const { return m_noteSpeed; }

    double GetJudgeOffsetSec() const { return m_judgeOffsetSec; }
    double GetPerfectRangeSec() const { return m_perfectRangeSec; }
    double GetGoodRangeSec() const { return m_goodRangeSec; }
    double GetBadRangeSec() const { return m_badRangeSec; }

    float GetLeadTimeSec() const
    {
        return (m_spawnZ - m_judgeZ) / m_noteSpeed;
    }

private:
    int m_laneCount = 5;
	float m_laneWidth = 2.0f;

    float m_spawnZ = 40.0f;
    float m_judgeZ = 0.0f;
    float m_playerZ = -4.0f;
    float m_passZ = -5.0f;
    float m_noteSpeed = 10.0f;

    double m_judgeOffsetSec = 0.05;
    double m_perfectRangeSec = 0.05;
    double m_goodRangeSec = 0.10;
    double m_badRangeSec = 0.15;
};