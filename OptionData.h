#pragma once

struct OptionData
{
	float NotesSpeed;
	float JudgeTiming;
	float MasterVolume;
	float MusicVolume;
	float SeVolume;
};

extern OptionData gOptionData;

const float kNotesSpeedRate = 4.0f;

inline float GetActualNotesSpeed()
{
	return gOptionData.NotesSpeed * kNotesSpeedRate;
}