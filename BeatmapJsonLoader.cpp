#include "BeatmapJsonLoader.h"
#include<fstream>
#include "Engine/nlohmann/json.hpp"

using json = nlohmann::json;


bool BeatmapJsonLoader::LoadFromFile(const std::string path, BeatMap& out, std::string* error)
{
    try {
        std::ifstream ifs(path);
        if (!ifs) {
            if (error) *error = "Failed to open: " + path;
            return false;
        }

        json j = json::parse(ifs);

        out.name = j.value("name", "");
        out.maxBlock = j.value("maxBlock", 0);
        out.bpm = j.value("BPM", 120);
        out.offset = j.value("offset", 0);

        out.notes.clear();
        if (j.contains("notes") && j["notes"].is_array()) {
            for (const auto& n : j["notes"]) {
                Note note;
                note.lpb = n.value("LPB", 4);
                note.num = n.value("num", 0);
                note.block = n.value("block", 0);
                note.type = n.value("type", 1);

                
                if (note.block < 0) continue;
                out.notes.push_back(note);
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        if (error) *error = e.what();
        return false;
    }

}
