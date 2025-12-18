#pragma once
#include"BeatmapMeta.h"
#include<vector>
#include<string>

class BeatmapCatalog
{
public:
	static std::vector<BeatmapMeta> LoadCatalog(const std::string& dir);
};

