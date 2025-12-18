#include "Notes.h"
#include "VerticalBeam.h"
#include "BesideBeam.h"
#include "Lane.h"
#include "BeatmapJsonLoader.h"
#include"Engine/Time.h"
#include <cassert>
#include <string>
#include<algorithm>


namespace
{
    static float TimeToZ(float sec)
    {
        const float speed = 6.0f;   // 調整用
        return sec * speed;
    }

    static float CalcNoteSec(int num, int lpb, int bpm, int offsetMs)
    {
        const double beatSec = 60.0 / (double)bpm;
        const double stepSec = beatSec / (double)lpb;
        const double t = (double)offsetMs / 1000.0 + (double)num * stepSec;
        return (float)t;
    }

    constexpr float SplitY = 0.7f; // 上下の間隔（見た目で調整）

    struct LanePlace {
        int laneIndex;   // 0..4
        float yOffset;   // 上下
        bool useBeside;  // BesideBeamにするか
    };

    // block 0..6 を 5レーン+上下 に割り当て
    // 0..2: lane1..3 通常（縦）
    // 3,4 : lane4 上/下（横）
    // 5,6 : lane5 上/下（横）
    static LanePlace DecodeBlock(int block)
    {
        if (block <= 2) return { block, 0.0f, false };           // lane1-3
        if (block == 3) return { 3, +SplitY, true };            // lane4 upper
        if (block == 4) return { 3, -SplitY, true };            // lane4 lower
        if (block == 5) return { 4, +SplitY, true };            // lane5 upper
        return              { 4, -SplitY, true };               // lane5 lower
    }

    constexpr float kJudgeZ = 0.0f;
    constexpr float kSpawnZ = 40.0f;
    constexpr float kScrollSpeed = 10.0f;
    constexpr double kLeadTimeSec = (kSpawnZ - kJudgeZ) / kScrollSpeed;
}

Notes::Notes(GameObject* parent)
    : GameObject(parent, "Notes")
{
}

Notes::~Notes() {}

void Notes::Initialize()
{
    std::string err;

    const std::string path = "Json/01 - Chartreuse Green.json";

    if (!BeatmapJsonLoader::LoadFromFile(path, map_, &err)) {
        assert(false);
        return;
    }
    char buf[256];
    sprintf_s(buf, "loaded notes = %zu\n", map_.notes.size());
    OutputDebugStringA(buf);

    nextIndex_ = 0;
    nowSec_ = 0.0;

    // 念のため num順（=時刻順）に並べる
    std::sort(map_.notes.begin(), map_.notes.end(),
        [](const auto& a, const auto& b) { return a.num < b.num; });
}

void Notes::Update()
{
    // ★曲の再生時間が取れない場合：dtを積算
    // エンジンに Time::DeltaTime() があるならそれを使うのが普通
    nowSec_ += Time::DeltaTime();

    // まだ譜面が無い or もう全部出し切った
    if (map_.notes.empty() || nextIndex_ >= map_.notes.size()) return;

    // 先読みして生成（同フレームで複数生成されるので while）
    while (nextIndex_ < map_.notes.size())
    {
        const auto& e = map_.notes[nextIndex_];

        const double hitSec = CalcNoteSec(e.num, e.lpb, map_.bpm, map_.offset);

        // 叩く leadTimeSec 秒前になったら生成
        if (nowSec_ < hitSec - kLeadTimeSec) break;

        // --- ここから生成処理 ---
        int laneIndex = e.block; // 0..4想定（上下分けなどするならここで変換）

        Lane* lane = Lane::FindByName("lane" + std::to_string(laneIndex + 1));
        if (lane)
        {
            XMFLOAT3 pos = lane->GetCenterPosition();
            pos.z = kSpawnZ;

            // 例：上下分けやBesideBeam判定があるならここで pos.y や種類を変える
            // const auto place = DecodeBlock(e.block);
            // pos.y += place.yOffset;

            if (e.type == 2) {
                auto* b = Instantiate<BesideBeam>(this);
                b->SetPosition(pos);
            }
            else {
                auto* v = Instantiate<VerticalBeam>(this);
                v->SetPosition(pos);
                // v->SetLaneIndex(laneIndex); // 必要なら
            }
        }

        ++nextIndex_;
    }
}void Notes::Draw() {}
void Notes::Release() {}
