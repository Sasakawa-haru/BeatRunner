// JsonToCsvConverter.cpp
#include "JsonToCsvConverter.h"

#include <json.hpp>
#include <filesystem>
#include <fstream>
#include <array>
#include <map>
#include <cmath>
#include <iomanip>

using json = nlohmann::json;
namespace fs = std::filesystem;

static void WriteUtf8Bom(std::ofstream& ofs)
{
    ofs << "\xEF\xBB\xBF";
}

static std::string CsvEscape(const std::string& s)
{
    bool needQuote = false;
    for (char c : s) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') { needQuote = true; break; }
    }
    if (!needQuote) return s;

    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out.push_back('"');
    return out;
}

// block が 0..6 でも 1..7 でも lane0(0..6) に変換
static int BlockToLaneIndex0(int block)
{
    if (0 <= block && block <= 6) return block;
    if (1 <= block && block <= 7) return block - 1;
    return -1;
}

// 指定式：timeSec = (num/LPB) * (60/BPM)
static double CalcTimeSec(int num, int lpb, int bpm)
{
    if (lpb <= 0 || bpm <= 0) return 0.0;
    return (double)num / (double)lpb * (60.0 / (double)bpm);
}

// jsonがcsvより新しい（or csv無し）なら true
static bool ShouldConvert(const fs::path& jsonPath, const fs::path& metaCsv, const fs::path& notesCsv)
{
    if (!fs::exists(metaCsv) || !fs::exists(notesCsv)) return true;

    const auto tJson = fs::last_write_time(jsonPath);
    const auto tMeta = fs::last_write_time(metaCsv);
    const auto tNotes = fs::last_write_time(notesCsv);

    return (tMeta < tJson) || (tNotes < tJson);
}

bool ConvertBeatmapJsonToCsvSeparated(
    const std::string& jsonPath,
    const std::string& metaCsvPath,
    const std::string& notesCsvPath,
    std::string* err)
{
    try {
        // --- JSON読み込み ---
        std::ifstream ifs(jsonPath);
        if (!ifs) { if (err) *err = "open failed: " + jsonPath; return false; }

        json j = json::parse(ifs);

        // --- meta取得 ---
        const std::string name = j.value("name", "");
        const int bpm = j.value("BPM", 120);
        const int offsetMs = j.value("offset", 0);
        const int maxBlock = j.value("maxBlock", 0);

        // --- meta.csv 出力（上書き） ---
        {
            std::ofstream meta(metaCsvPath, std::ios::binary);
            if (!meta) { if (err) *err = "open failed: " + metaCsvPath; return false; }

            WriteUtf8Bom(meta);
            meta << "state,value\n";
            meta << "name," << CsvEscape(name) << "\n";
            meta << "BPM," << bpm << "\n";
            meta << "offset," << offsetMs << "\n";
            meta << "maxBlock," << maxBlock << "\n";
        }

        // --- notesテーブル：time(ms) -> lane1..7(0/1) ---
        std::map<long long, std::array<int, 7>> table;

        if (j.contains("notes") && j["notes"].is_array()) {
            for (const auto& n : j["notes"]) {
                const int lpb = n.value("LPB", 4);
                const int num = n.value("num", 0);
                const int block = n.value("block", -1);

                const int lane0 = BlockToLaneIndex0(block);
                if (lane0 < 0 || lane0 >= 7) continue;

                const double tSec = CalcTimeSec(num, lpb, bpm);
                const long long tMs = (long long)std::llround(tSec * 1000.0); // 1ms丸め

                auto it = table.find(tMs);
                if (it == table.end()) {
                    std::array<int, 7> zeros = { 0,0,0,0,0,0,0 };
                    it = table.emplace(tMs, zeros).first;
                }
                it->second[lane0] = 1; // 同時刻はOR
            }
        }

        // --- notes.csv 出力（上書き） ---
        {
            std::ofstream notes(notesCsvPath, std::ios::binary);
            if (!notes) { if (err) *err = "open failed: " + notesCsvPath; return false; }

            WriteUtf8Bom(notes);
            notes << "time,lane1,lane2,lane3,lane4,lane5,lane6,lane7\n";

            for (const auto& [tMs, lanes] : table) {
                const double tSec = (double)tMs / 1000.0;

                notes << std::fixed << std::setprecision(3) << tSec;
                for (int i = 0; i < 7; ++i) notes << "," << lanes[i];
                notes << "\n";
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        if (err) *err = e.what();
        return false;
    }
}

void ConvertAllJsonInDir_IfJsonNewer(const std::string& inDir)
{
    fs::path inputDir(inDir);
    if (!fs::exists(inputDir)) return;

    // ★ 出力先フォルダ
    const fs::path outMetaDir = fs::path("Csv")/"Meta";
    const fs::path outNotesDir = fs::path("Csv") / "Notes";

    fs::create_directories(outMetaDir);
    fs::create_directories(outNotesDir);

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (!entry.is_regular_file()) continue;

        const fs::path jsonPath = entry.path();
        if (jsonPath.extension() != ".json") continue;

        const std::string stem = jsonPath.stem().string();

        //  出力先：Assets/Csv/Meta と Assets/Csv/Notes
        const fs::path metaCsv = outMetaDir / (stem + "_meta.csv");
        const fs::path notesCsv = outNotesDir / (stem + "_notes.csv");

        // 更新日時チェック
        if (!ShouldConvert(jsonPath, metaCsv, notesCsv)) continue;

        std::string err;
        ConvertBeatmapJsonToCsvSeparated(jsonPath.string(), metaCsv.string(), notesCsv.string(), &err);
    }
}
