#include "Option.h"
#include"Engine/Input.h"

Option::Option(GameObject* parent)
	:GameObject(parent,"Option")
	,option_(NOTES_SPEED)
{
}

Option::~Option()
{
}

void Option::Initialize()
{
}

void Option::Update()
{
	if (Input::IsKeyDown(DIK_UP)) {
		if (option_ == NOTES_SPEED) {
			option_ = SE_VOLUME;
		}
		else {
			option_ = (OPTION)(option_ + 1);
		}
	}
	if (Input::IsKeyDown(DIK_DOWN)) {
		if (option_ = SE_VOLUME) {
			option_ = NOTES_SPEED;
		}
		else {
			option_ = (OPTION)(option_ - 1);
		}
	}
	if (Input::IsKeyDown(DIK_LEFT)) {
		if (option_ == NOTES_SPEED) {
			if (NotesSpeed > 0) {
				NotesSpeed = NotesSpeed-0.1;
			}
		}
		if (option_ == JUDGE_TIMING) {
			JudgeTiming = JudgeTiming-0.1f;
		}
		if (option_ == MASTER_VOLUME) {
			if (MasterVolume >= 0.0f) {
				MasterVolume = MasterVolume-0.1;
			}
		}
		if (option_ == MUSIC_VOLUME) {
			if (MusicVolume >= 0.0f) {
				MusicVolume = MusicVolume-0.1;
			}
		}
		if (option_ == SE_VOLUME) {
			if (SeVolume >= 0.0f) {
				SeVolume = SeVolume-0.1;
			}
		}

	}
	if (Input::IsKeyDown(DIK_RIGHT)) {
		if (option_ == NOTES_SPEED) {
			if (NotesSpeed < 10) {
				NotesSpeed = NotesSpeed + 0.1;
			}
		}
		if (option_ == JUDGE_TIMING) {
			JudgeTiming = JudgeTiming + 0.1f;
		}
		if (option_ == MASTER_VOLUME) {
			if (MasterVolume <= 1.0f) {
				MasterVolume = MasterVolume + 0.1f;
			}
		}
		if (option_ == MUSIC_VOLUME) {
			if (MusicVolume <= 1.0f) {
				MusicVolume = MusicVolume + 0.1f;
			}
		}
		if (option_ == SE_VOLUME) {
			if (SeVolume <= 1.0f) {
				SeVolume = SeVolume + 0.1f;
			}
		}

	}

}

void Option::Draw()
{
}

void Option::Release()
{
}
