#pragma once
#include<string>

struct BeatmapMeta {
	std::string path;
	std::string name;
	int bpm;
	int offsetMs;
	int maxBlock;
};