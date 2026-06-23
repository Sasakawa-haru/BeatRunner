#include "Option.h"
#include"OptionData.h"
#include"Engine//Audio.h"
#include"Engine/Input.h"
#include"Engine/Text.h"
#include"Engine/Image.h"

OptionData gOptionData = {
	5.0f,//NotesSpeed
	0.0f,//JudgeTiming
	1.0f,//MasterVolume
	1.0f,//MusicVolume
	0.5f,//BGMVolume
	1.0f//SeVolume
};


Option::Option(GameObject* parent)
	:GameObject(parent,"Option")
	,option_(NOTES_SPEED)
	,hBackGroundOption_(-1)
{
}

Option::~Option()
{
}

void Option::Initialize()
{
	hBackGroundOption_ = Image::Load("Image/OptionBackGround.png");
	assert(hBackGroundOption_ >= 0);
	pText_ = new Text;
	pText_->Initialize();

	NotesSpeed = gOptionData.NotesSpeed;
	JudgeTiming = gOptionData.JudgeTiming;
	MasterVolume = gOptionData.MasterVolume;
	MusicVolume = gOptionData.MusicVolume;
	SeVolume = gOptionData.SeVolume;

	//Audio::SetMasterVolume(gOptionData.MasterVolume);
}

void Option::Update()
{
	if (OptionMode) {
		if (Input::IsKeyDown(DIK_UP)) {
			if (option_ == NOTES_SPEED) {
				option_ = SE_VOLUME;
			}
			else {
				option_ = (OPTION)(option_ - 1);
			}
		}
		if (Input::IsKeyDown(DIK_DOWN)) {
			if (option_ == SE_VOLUME) {
				option_ = NOTES_SPEED;
			}
			else {
				option_ = (OPTION)(option_ +1);
			}
		}
		if (Input::IsKeyDown(DIK_LEFT)) {
			if (option_ == NOTES_SPEED) {
				if (NotesSpeed > 0) {
					NotesSpeed = NotesSpeed - 0.1;
				}
			}
			if (option_ == JUDGE_TIMING) {
				JudgeTiming = JudgeTiming - 0.1f;
			}
			if (option_ == MASTER_VOLUME) {
				if (MasterVolume > 0.0f) {
					MasterVolume = MasterVolume - 0.1;
				}
			}
			if (option_ == MUSIC_VOLUME) {
				if (MusicVolume > 0.0f) {
					MusicVolume = MusicVolume - 0.1;
				}
			}
			if (option_ == BGM_VOLUME) {
				if (BGMVolume > 0.0f) {
					BGMVolume = BGMVolume - 0.1;
				}
			}
			if (option_ == SE_VOLUME) {
				if (SeVolume > 0.0f) {
					SeVolume = SeVolume - 0.1;
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
				if (MasterVolume < 1.0f) {
					MasterVolume = MasterVolume + 0.1f;
				}
			}
			if (option_ == MUSIC_VOLUME) {
				if (MusicVolume < 1.0f) {
					MusicVolume = MusicVolume + 0.1f;
				}
			}
			if (option_ == BGM_VOLUME) {
				if (BGMVolume < 1.0f) {
					BGMVolume = BGMVolume + 0.1f;
				}
			}
			if (option_ == SE_VOLUME) {
				if (SeVolume < 1.0f) {
					SeVolume = SeVolume + 0.1f;
				}
			}

		}
	}
}

void Option::Draw()
{
	if (OptionMode) {
		//背景
		Image::SetAlpha(hBackGroundOption_, 128);
		Image::SetTransform(hBackGroundOption_,transform_);
		Image::Draw(hBackGroundOption_);

		int x = 200;
		int y = 150;
		int interval = 40;

		//タイトル
		pText_->Draw(x, y - 60, "OPTION");

		// カーソル
		pText_->Draw(x - 40, y + option_ * interval, ">");

		// 項目名
		pText_->Draw(x, y + NOTES_SPEED * interval, "Notes Speed");
		pText_->Draw(x, y + JUDGE_TIMING * interval, "Judge Timing");
		pText_->Draw(x, y + MASTER_VOLUME * interval, "Master Volume");
		pText_->Draw(x, y + MUSIC_VOLUME * interval, "Music Volume");
		pText_->Draw(x, y + BGM_VOLUME * interval, "BGM Volume");
		pText_->Draw(x, y + SE_VOLUME * interval, "SE Volume");

		// 数値表示
		char buf[64];

		sprintf_s(buf, "%.1f", NotesSpeed);
		pText_->Draw(x + 250, y + NOTES_SPEED * interval, buf);

		sprintf_s(buf, "%.1f", JudgeTiming);
		pText_->Draw(x + 250, y + JUDGE_TIMING * interval, buf);

		sprintf_s(buf, "%.1f", MasterVolume);
		pText_->Draw(x + 250, y + MASTER_VOLUME * interval, buf);

		sprintf_s(buf, "%.1f", MusicVolume);
		pText_->Draw(x + 250, y + MUSIC_VOLUME * interval, buf);

		sprintf_s(buf, " %.1f", BGMVolume);
		pText_->Draw(x + 250, y + BGM_VOLUME * interval, buf);

		sprintf_s(buf, "%.1f", SeVolume);
		pText_->Draw(x + 250, y + SE_VOLUME * interval, buf);
	}
	

}

void Option::Release()
{
	if (pText_ != nullptr) {
		delete pText_;
		pText_ = nullptr;
	}
}

void Option::ApplyOptionData()
{
	gOptionData.NotesSpeed = NotesSpeed;
	gOptionData.JudgeTiming = JudgeTiming;
	gOptionData.MasterVolume = MasterVolume;
	gOptionData.MusicVolume = MusicVolume;
	gOptionData.BGMVolume = BGMVolume;
	gOptionData.SeVolume = SeVolume;
}

