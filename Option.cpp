#include "Option.h"
#include"Engine/Input.h"

Option::Option(GameObject* parent):option_(NOTES_SPEED)
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
				NotesSpeed = -0.1;
			}
		}
		if (option_ == JUDGE_TIMING) {
			JudgeTiming = -0.1f;
		}
	}
}

void Option::Draw()
{
}

void Option::Release()
{
}
