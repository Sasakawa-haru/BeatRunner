#include "BeatmapCatalog.h"
#include<filesystem>
#include<fstream>
#include "Engine/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::vector<BeatmapMeta> BeatmapCatalog::LoadCatalog(const std::string& dir)
{
    std::vector<BeatmapMeta> out;

    if (!fs::exists(dir)) return out;

    for (auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".json") continue;

        try {
            std::ifstream ifs(entry.path());
            if (!ifs) continue;

            json j = json::parse(ifs);

            BeatmapMeta m;
            m.path = entry.path().string();
            m.name = j.value("name", entry.path().stem().string());
            m.bpm = j.value("BPM", 120);
            m.offsetMs = j.value("offset", 0);
            m.maxBlock = j.value("maxBlock", 0);

            out.push_back(std::move(m));
        }
        catch (...) {
            // ‰ó‚ê‚Ä‚éjson‚Í–³Ž‹
        }
    }
    return out;
}