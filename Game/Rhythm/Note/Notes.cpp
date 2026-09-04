#include "Game/Rhythm/Note/Notes.h"

#include "Engine/Time.h"
#include "Engine/GameCsvReader.h"

#include "Game/Rhythm/Lane/Lane.h"
#include "Game/Rhythm/Lane/RhythmLayout.h"
#include "Game/Rhythm/Note/RhythmNote.h"
#include "Game/Rhythm/Music/Music.h"
#include "Game/Rhythm/Music/SelectedMusic.h"
#include "Game/Config/OptionData.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace
{
    // Notes共通設定

    // ノーツ速度が0以下になると移動時間の計算が壊れるため、
    // 使用できる最低速度を設定する
    constexpr float kMinNotesSpeed = 0.1f;

    // CSV設定

    // CSVの0列目はノーツの判定時間
    constexpr int kHitTimeColumn = 0;

    // ノーツ情報は1列目から始まる
    constexpr int kFirstLaneColumn = 1;

    // CSVの1行目以降が譜面データ
    constexpr int kFirstDataLine = 1;

    // CSV上でノーツが存在することを表す値
    constexpr int kNoteExistsFlag = 1;

    // ヘッダーとして使用する行数
    constexpr int kHeaderLineCount = 1;

    // レーン設定

    // 実際に画面上に存在するレーン数
    constexpr int kVisibleLaneCount = 5;

    // レーン番号の最小値
    constexpr int kFirstLaneIndex = 0;

    // Lane::FindByName() の名前は lane1 から始まるため、
    // 内部インデックスとの差を補正する
    constexpr int kLaneNameOffset = 1;

    // CSV上のジャンプノーツ用レーン
    //
    // lane1～lane5:
    //   通常の5レーン
    //
    // lane6:
    //   ジャンプで避けるノーツ
    //
    // 配列インデックスでは0始まりなので5
    constexpr int kJumpNoteLaneIndex = 5;

    // ジャンプノーツを配置する基準レーン
    // 5レーンの中央であるlane3
    //
    // 配列インデックスでは0始まりなので2
    constexpr int kJumpNoteBaseLaneIndex = 2;

    // ノーツ高さ

    // 通常ノーツのY方向オフセット
    constexpr float kVerticalNoteHeightOffset = 2.0f;

    // ジャンプ回避ノーツのY方向オフセット
    constexpr float kJumpNoteHeightOffset = 1.0f;

    // グループID計算

    // 秒からミリ秒へ変換する倍率
    constexpr float kMillisecondsPerSecond = 1000.0f;

    // float → int変換時の四捨五入用
    constexpr float kRoundOffset = 0.5f;

    // グループが存在しない場合に返す値
    constexpr int kInvalidGroupId = -1;

    // ノーツ数が取得できなかった場合に返す最低値
    constexpr int kMinimumNotesCount = 1;
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
    const std::string musicLevelPath =
        "Csv/Notes/"
        + gSelectedMusicName
        + "_"
        + gSelectedMusicLevel
        + ".csv";

    notesCsv_ =
        std::make_unique<GameCsvReader>(
            musicLevelPath.c_str()
        );

    nowSec_ = 0.0;

    // 0行目はヘッダーなので、
    // 1行目からノーツを読み込む
    nextLine_ = kFirstDataLine;

    laneCount_ = 0;

    if (notesCsv_ && notesCsv_->GetLines() > 0)
    {
        // 0列目は判定時間なので、
        // それ以外をレーン数として扱う
        laneCount_ =
            notesCsv_->GetColumns(kHitTimeColumn)
            - kFirstLaneColumn;

        if (laneCount_ < 0)
        {
            laneCount_ = 0;
        }
    }

    BuildGroupsFromCsv();
}


void Notes::Update()
{
    Music* music =
        (Music*)FindObject("Music");

    if (!music || !music->IsStarted())
    {
        return;
    }

    if (!notesCsv_)
    {
        return;
    }

    // Optionで設定した判定タイミング調整を反映する
    nowSec_ =
        music->GetNowSec()
        + gOptionData.JudgeTiming;

    const int lines =
        notesCsv_->GetLines();

    if (nextLine_ >= lines)
    {
        return;
    }

    // Optionで設定したノーツ速度を取得する
    float notesSpeed =
        GetActualNotesSpeed();

    // 速度が極端に小さい、または0以下の場合の安全対策
    if (notesSpeed < kMinNotesSpeed)
    {
        notesSpeed = kMinNotesSpeed;
    }

    // ノーツがSpawnZからJudgeLineZまで
    // 移動するために必要な時間
    const double leadTimeSec =
        (RhythmLayout::SpawnZ - RhythmLayout::JudgeLineZ)
        / notesSpeed;

    while (nextLine_ < lines)
    {
        const float hitTimeSec =
            notesCsv_->GetFloat(
                nextLine_,
                kHitTimeColumn
            );

        // まだ生成時刻に到達していない場合、
        // これ以降のノーツもまだ生成しない
        if (nowSec_ < hitTimeSec - leadTimeSec)
        {
            break;
        }

        for (int lane = 0; lane < laneCount_; ++lane)
        {
            const int noteColumn =
                kFirstLaneColumn + lane;

            const int noteFlag =
                notesCsv_->GetInt(
                    nextLine_,
                    noteColumn
                );

            // このレーンにノーツが無ければ次へ
            if (noteFlag != kNoteExistsFlag)
            {
                continue;
            }

            bool isJumpNote = false;

            // 通常ノーツの場合はCSV上のレーンをそのまま使う
            int baseLane = lane;

            // =================================================
            // ジャンプ回避ノーツ判定
            // =================================================

            if (lane == kJumpNoteLaneIndex)
            {
                isJumpNote = true;

                // ジャンプノーツは5レーン全体を横断するため、
                // 中央レーンを基準位置として生成する
                baseLane = kJumpNoteBaseLaneIndex;
            }
            // レーン範囲チェック

            if (baseLane < kFirstLaneIndex
                || baseLane >= kVisibleLaneCount)
            {
                continue;
            }

            const std::string laneName =
                "lane"
                + std::to_string(
                    baseLane + kLaneNameOffset
                );

            Lane* laneObject =
                Lane::FindByName(laneName);

            if (!laneObject)
            {
                continue;
            }

            // ノーツ位置設定

            XMFLOAT3 position =
                laneObject->GetCenterPosition();

            if (isJumpNote)
            {
                position.y +=
                    kJumpNoteHeightOffset;
            }
            else
            {
                position.y +=
                    kVerticalNoteHeightOffset;
            }

            // 判定時間から現在位置を逆算する
            position.z =
                RhythmLayout::JudgeLineZ
                + (hitTimeSec - nowSec_)
                * notesSpeed;

            // RhythmNote生成

            RhythmNote* note =
                Instantiate<RhythmNote>(this);

            if (!note)
            {
                continue;
            }

            if (isJumpNote)
            {
                note->Setup(
                    NotesType::JumpNote
                );
            }
            else
            {
                note->Setup(
                    NotesType::VerticalNote
                );
            }

            note->SetPosition(position);

            note->SetLane(baseLane);

            note->SetHitTimeSec(
                hitTimeSec
            );

            // =================================================
            // 同じ判定時間のノーツを同グループにする
            // =================================================

            const int timeMs =
                static_cast<int>(
                    hitTimeSec
                    * kMillisecondsPerSecond
                    + kRoundOffset
                    );

            note->SetGroupId(
                GetGroupIdByTimeMs(timeMs)
            );
        }

        ++nextLine_;
    }
}


void Notes::Draw()
{
}


void Notes::Release()
{
}


int Notes::GetGroupIdByTimeMs(int timeMs) const
{
    const auto it =
        timeMsToGroupId_.find(timeMs);

    if (it == timeMsToGroupId_.end())
    {
        return kInvalidGroupId;
    }

    return it->second;
}


int Notes::GetAllNotesCount() const
{
    if (!notesCsv_)
    {
        return kMinimumNotesCount;
    }

    const int lines =
        notesCsv_->GetLines();

    const int notesCount =
        lines - kHeaderLineCount;

    if (notesCount < 0)
    {
        return kMinimumNotesCount;
    }

    return notesCount;
}


void Notes::BuildGroupsFromCsv()
{
    groupTimesMs_.clear();

    timeMsToGroupId_.clear();

    if (!notesCsv_)
    {
        return;
    }

    const int lines =
        notesCsv_->GetLines();

    if (lines <= kHeaderLineCount)
    {
        return;
    }

    for (
        int line = kFirstDataLine;
        line < lines;
        ++line
        )
    {
        const float hitTimeSec =
            notesCsv_->GetFloat(
                line,
                kHitTimeColumn
            );

        const int timeMs =
            static_cast<int>(
                hitTimeSec
                * kMillisecondsPerSecond
                + kRoundOffset
                );

        groupTimesMs_.push_back(timeMs);
    }

    // 時間順に並べる
    std::sort(
        groupTimesMs_.begin(),
        groupTimesMs_.end()
    );

    // 同じ時間を1つにまとめる
    groupTimesMs_.erase(
        std::unique(
            groupTimesMs_.begin(),
            groupTimesMs_.end()
        ),
        groupTimesMs_.end()
    );

    // 判定時間 → グループID
    for (
        int groupId = 0;
        groupId < static_cast<int>(groupTimesMs_.size());
        ++groupId
        )
    {
        timeMsToGroupId_[
            groupTimesMs_[groupId]
        ] = groupId;
    }
}