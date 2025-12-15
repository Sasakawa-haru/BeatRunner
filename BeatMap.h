#pragma once
#include <string>
#include <vector>

struct Note {
	int lpb = 4;
	int num = 0;
	int block = 0;
	int type = 1;
};
struct BeatMap
{
	std::string name;
	int maxBlock = 0;
	int bpm = 120;
	int offset = 0;
	std::vector<Note> notes;
};

