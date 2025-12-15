#pragma once
#include "BeatMap.h"
#include<string>

class BeatmapJsonLoader
{
public:
	static bool LoadFromFile(const std::string path, BeatMap& out, std::string* error = nullptr);
};

