#include "GameConfig.h"

#include "Engine/GameCsvReader.h"

#include <iostream>
#include <string>

bool GameConfig::LoadFromCsv(const std::string& filePath)
{
    GameCsvReader reader(filePath);

    if (reader.GetLines() <= 1)
    {
        std::cout << "GameConfig csv load failed: " << filePath << std::endl;
        return false;
    }

	// 1行目はヘッダなので、2行目以降を読み込む
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
        std::cout << "LaneCount error" << std::endl;
        return false;
    }

    if (m_laneWidth <= 0.0f)
    {
        std::cout << "LaneWidth error" << std::endl;
        return false;
    }

    if (m_noteSpeed <= 0.0f)
    {
		std::cout << "NoteSpeed error" << std::endl;//ノーツ速度は0以下にできない
        return false;
    }

    if (m_spawnZ <= m_judgeZ)
    {
		std::cout << "SpawnZ JudgeZ error" << std::endl;//スポーン位置は判定位置より奥にする必要がある
        return false;
    }

    if (m_perfectRangeSec < 0.0 ||
        m_goodRangeSec < 0.0 ||
        m_badRangeSec < 0.0)
    {
        std::cout << "Judge range error" << std::endl;//判定範囲は0未満にできない
        return false;
    }

    return true;
}