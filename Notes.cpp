#include "Notes.h"

#include "Engine/Time.h"
#include "Engine/GameCsvReader.h"
#include "Lane.h"
#include "Beam.h"
#include "Music.h"
#include "SelectedMusic.h"
#include "OptionData.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace
{
    // 判定ラインのZ座標
    constexpr float kJudgeZ = 0.0f;

    // ノーツを出現させるZ座標
    constexpr float kSpawnZ = 50.0f;

    // 速度が0以下になると計算が壊れるので最低速度を決める
    constexpr float kMinNotesSpeed = 0.1f;

    // lane6 / lane7 を上下にずらす量
    constexpr float kSplitY = 0.3f;

    constexpr float kBesideOffsetX = -4.5f;
}

Notes::Notes(GameObject* parent)
    : GameObject(parent, "Notes")
{
}

Notes::~Notes()
{
}

void Notes::Initialize()
{
    std::string musicLevelPath =
        "Csv/Notes/" + gSelectedMusicName + "_" + gSelectedMusicLevel + ".csv";

    notesCsv_ = std::make_unique<GameCsvReader>(musicLevelPath.c_str());

    nowSec_ = 0.0;
    nextLine_ = 1;

    laneCount_ = 0;

    if (notesCsv_ && notesCsv_->GetLines() > 0)
    {
        // 0列目は時間なので、それ以外がレーン数
        laneCount_ = notesCsv_->GetColumns(0) - 1;

        if (laneCount_ < 0)
        {
            laneCount_ = 0;
        }
    }

    BuildGroupsFromCsv();
}

void Notes::Update()
{
    Music* music = (Music*)FindObject("Music");

    if (!music || !music->IsStarted())
    {
        return;
    }

    if (!notesCsv_)
    {
        return;
    }

    // Optionで設定した判定調整を反映
    nowSec_ = music->GetNowSec() + gOptionData.JudgeTiming;

    const int lines = notesCsv_->GetLines();

    if (nextLine_ >= lines)
    {
        return;
    }

    // Optionで設定したノーツ速度を反映
    float notesSpeed = GetActualNotesSpeed();

    if (notesSpeed < kMinNotesSpeed)
    {
        notesSpeed = kMinNotesSpeed;
    }

    // ノーツが SpawnZ から JudgeZ まで来るのに必要な時間
    const double leadTimeSec = (kSpawnZ - kJudgeZ) / notesSpeed;

    while (nextLine_ < lines)
    {
        const float hitTimeSec = notesCsv_->GetFloat(nextLine_, 0);

        // まだ出す時間ではないなら終了
        if (nowSec_ < hitTimeSec - leadTimeSec)
        {
            break;
        }

        for (int lane = 0; lane < laneCount_; lane++)
        {
            const int noteFlag = notesCsv_->GetInt(nextLine_, 1 + lane);

            if (noteFlag != 1)
            {
                continue;
            }

            bool isBeside = false;
            int baseLane = lane;
            float yOff = 0.0f;

            // lane1～lane5 は VerticalBeam
            // lane6 / lane7 は lane5 の上下に BesideBeam として出す
            if (lane == 5 || lane == 6)
            {
                isBeside = true;
                baseLane = 4;
                yOff = (lane == 5) ? +kSplitY : -kSplitY;
            }

            // 実際に画面上にあるレーンは lane1～lane5 想定
            if (baseLane < 0 || baseLane >= 5)
            {
                continue;
            }

            Lane* ln = Lane::FindByName("lane" + std::to_string(baseLane + 1));

            if (!ln)
            {
                continue;
            }

            XMFLOAT3 pos = ln->GetCenterPosition();

            pos.x += Lane::laneWidth * 0.5f;
            pos.y += yOff + 2.0f;
            pos.z = kSpawnZ;

            auto* note = Instantiate<Beam>(this);

            if (isBeside)
            {
                pos.x += kBesideOffsetX;
                note->Setup(BeamType::Beside);
            }
            else
            {
                note->Setup(BeamType::Vertical);
            }

            note->SetPosition(pos);
            note->SetLane(baseLane);
            note->SetHitTimeSec(hitTimeSec);
        }

        nextLine_++;
    }
}

void Notes::Draw()
{
}

void Notes::Release()
{
}

int Notes::GetGroupIdByTimeMs(int tms) const
{
    auto it = timeMsToGroupId_.find(tms);

    if (it == timeMsToGroupId_.end())
    {
        return -1;
    }

    return it->second;
}

void Notes::BuildGroupsFromCsv()
{
    groupTimesMs_.clear();
    timeMsToGroupId_.clear();

    if (!notesCsv_)
    {
        return;
    }

    const int lines = notesCsv_->GetLines();

    if (lines <= 1)
    {
        return;
    }

    for (int line = 1; line < lines; ++line)
    {
        const float hitTimeSec = notesCsv_->GetFloat(line, 0);

        const int tms = static_cast<int>(hitTimeSec * 1000.0f + 0.5f);

        groupTimesMs_.push_back(tms);
    }

    std::sort(groupTimesMs_.begin(), groupTimesMs_.end());

    groupTimesMs_.erase(
        std::unique(groupTimesMs_.begin(), groupTimesMs_.end()),
        groupTimesMs_.end()
    );

    for (int i = 0; i < static_cast<int>(groupTimesMs_.size()); ++i)
    {
        timeMsToGroupId_[groupTimesMs_[i]] = i;
    }
}