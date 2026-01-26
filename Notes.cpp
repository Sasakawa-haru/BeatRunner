#include "Notes.h"

#include "Engine/Time.h"
#include "Engine/GameCsvReader.h"
#include "Lane.h"
#include "VerticalBeam.h"

#include <memory>
#include <string>

namespace
{
    constexpr float  kJudgeZ = 0.0f;
    constexpr float  kSpawnZ = 40.0f;
    constexpr float  kScrollSpeed = 10.0f;
    constexpr double kLeadTimeSec = (kSpawnZ - kJudgeZ) / kScrollSpeed;
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
        laneCount_ = notesCsv_->GetColumns(0) - 1; // timeóÒÇèúÇ≠
        if (laneCount_ < 0) laneCount_ = 0;
    }
}

void Notes::Update()
{
    nowSec_ += Time::DeltaTime();
    if (!notesCsv_) return;

    const int lines = notesCsv_->GetLines();
    while (nextLine_ < lines)
    {
        const float hitTimeSec = notesCsv_->GetFloat(nextLine_, 0);

        if (nowSec_ < hitTimeSec - kLeadTimeSec) break;

        for (int lane = 0; lane < laneCount_; lane++)
        {
            if (notesCsv_->GetInt(nextLine_, 1 + lane) != 1) continue;

            Lane* ln = Lane::FindByName("lane" + std::to_string(lane + 1));
            if (!ln) continue;

            XMFLOAT3 pos = ln->GetCenterPosition();
            pos.z = kSpawnZ;

            auto* note = Instantiate<VerticalBeam>(this);
            note->SetPosition(pos);
        }

        nextLine_++;
    }
}

void Notes::Draw() {}
void Notes::Release() {}