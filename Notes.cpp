// Notes.cpp（CSV譜面版：lane6/lane7 → lane5の上下にBesideBeam）
#include "Notes.h"

#include "Engine/Time.h"
#include "Engine/GameCsvReader.h"
#include "Lane.h"
#include "VerticalBeam.h"
#include "BesideBeam.h"

#include <memory>
#include <string>

namespace
{
    constexpr float  kJudgeZ = 0.0f;
    constexpr float  kSpawnZ = 40.0f;
    constexpr float  kScrollSpeed = 10.0f;
    constexpr double kLeadTimeSec = (kSpawnZ - kJudgeZ) / kScrollSpeed;

    constexpr float  kSplitY = 0.7f; // lane6/lane7 を lane5 の上下にずらす量
}

Notes::Notes(GameObject* parent)
    : GameObject(parent, "Notes")
{
}

Notes::~Notes() {}

void Notes::Initialize()
{
    notesCsv_ = std::make_unique<GameCsvReader>("Csv/Notes/01 - Chartreuse Green_notes.csv");

    nowSec_ = 0.0;
    nextLine_ = 1; 

    laneCount_ = 0;
    if (notesCsv_ && notesCsv_->GetLines() > 0) {
        laneCount_ = notesCsv_->GetColumns(0) - 1; // time列を除く
        if (laneCount_ < 0) laneCount_ = 0;
    }
}

void Notes::Update()
{
    nowSec_ += Time::DeltaTime();
    if (!notesCsv_) return;

    const int lines = notesCsv_->GetLines();
    if (nextLine_ >= lines) return;

    while (nextLine_ < lines)
    {
        const float hitTimeSec = notesCsv_->GetFloat(nextLine_, 0);
        if (nowSec_ < hitTimeSec - kLeadTimeSec) break;

        for (int lane = 0; lane < laneCount_; lane++)
        {
            if (notesCsv_->GetInt(nextLine_, 1 + lane) != 1) continue;

            bool isBeside = false;
            int baseLane = lane;   // 0..4（lane1..5）に落とす
            float yOff = 0.0f;

            // lane1..5 はそのまま VerticalBeam
            // lane6/lane7 は lane5 の上下に BesideBeam
            if (lane == 5 || lane == 6) {         // CSV: lane6/lane7（0-based）
                isBeside = true;
                baseLane = 4;                      // lane5（0-based）
                yOff = (lane == 5) ? +kSplitY : -kSplitY;
            }

            // 画面上のLaneは lane1..lane5 を想定
            if (baseLane < 0 || baseLane >= 5) continue;

            Lane* ln = Lane::FindByName("lane" + std::to_string(baseLane + 1));
            if (!ln) continue;

            XMFLOAT3 pos = ln->GetCenterPosition();
            pos.y += yOff;
            pos.z = kSpawnZ;

            if (isBeside) {
                auto* note = Instantiate<BesideBeam>(this);
                note->SetPosition(pos);
            }
            else {
                auto* note = Instantiate<VerticalBeam>(this);
                note->SetPosition(pos);
            }
        }

        nextLine_++;
    }
}

void Notes::Draw() {}
void Notes::Release() {}
