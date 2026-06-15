#include "GameConfig.h"
#include "Engine/GameCsvReader.h"

#include <iostream>
#include <string>

bool GameConfig::LoadFromCsv(const std::string& filePath)
{
    GameCsvReader reader(filePath);

    if (reader.GetLines() <= 1)
    {
        std::cout << "GameConfig CSVを読み込めません、または中身が空です: "
            << filePath << std::endl;
        return false;
    }

    // 0行目はヘッダー key,value として飛ばす
    for (int line = 1; line < reader.GetLines(); ++line)
    {
        if (reader.GetColumns(line) < 2)
        {
            continue;
        }

        const std::string key = reader.GetString(line, 0);

        if (key == "LaneCount")
        {
            m_laneCount = reader.GetInt(line, 1);
        }
        else if (key == "LaneWidth")
        {
            m_laneWidth = reader.GetFloat(line, 1);
        }
        else if (key == "SpawnZ")
        {
            m_spawnZ = reader.GetFloat(line, 1);
        }
        else if (key == "JudgeZ")
        {
            m_judgeZ = reader.GetFloat(line, 1);
        }
        else if (key == "PlayerZ")
        {
            m_playerZ = reader.GetFloat(line, 1);
        }
        else if (key == "PassZ")
        {
            m_passZ = reader.GetFloat(line, 1);
        }
        else if (key == "NoteSpeed")
        {
            m_noteSpeed = reader.GetFloat(line, 1);
        }
        else if (key == "JudgeOffsetSec")
        {
            m_judgeOffsetSec = reader.GetDouble(line, 1);
        }
        else if (key == "PerfectRangeSec")
        {
            m_perfectRangeSec = reader.GetDouble(line, 1);
        }
        else if (key == "GoodRangeSec")
        {
            m_goodRangeSec = reader.GetDouble(line, 1);
        }
        else if (key == "BadRangeSec")
        {
            m_badRangeSec = reader.GetDouble(line, 1);
        }
    }

    if (m_laneCount <= 0)
    {
        std::cout << "LaneCountが0以下です" << std::endl;
        return false;
    }

    if (m_noteSpeed <= 0.0f)
    {
        std::cout << "NoteSpeedが0以下です" << std::endl;
        return false;
    }

    if (m_spawnZ <= m_judgeZ)
    {
        std::cout << "SpawnZはJudgeZより大きくしてください" << std::endl;
        return false;
    }

    return true;
}