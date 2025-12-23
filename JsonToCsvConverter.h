#pragma once
#include <string>

// data/score/xxx.json -> data/score/xxx_meta.csv と xxx_notes.csv を生成/上書き
bool ConvertBeatmapJsonToCsvSeparated(
    const std::string& jsonPath,
    const std::string& metaCsvPath,
    const std::string& notesCsvPath,
    std::string* err = nullptr);

// フォルダ内の全jsonについて、jsonがcsvより新しい（or csvが無い）ときだけ変換
void ConvertAllJsonInDir_IfJsonNewer(const std::string& dir);
