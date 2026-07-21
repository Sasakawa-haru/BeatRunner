#pragma once
#include "BeatMap.h"
#include<string>

struct NoteEvent {
	int lpb = 4;
	int num = 0;
	int block = 0;
	int type = 1;
	std::vector<int>notes;
};

struct BeatmapData {
	std::string name;
	int maxBlock = 0;
	int bpm = 120;
	int offset = 0;
	std::vector<NoteEvent>events;
};

class BeatmapJsonLoader
{
public:
	static bool LoadFromFile(const std::string path, BeatMap& out, std::string* error = nullptr);
};

