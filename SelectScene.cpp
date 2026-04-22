#include "SelectScene.h"
#include"Music.h"
#include"Engine/Input.h"
#include"Engine/Image.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"
#include"Engine/Text.h"
#include"Engine/GameCsvReader.h"
#include"Engine/SceneManager.h"
#include"SelectedMusic.h"
#include<string>


SelectScene::SelectScene(GameObject* parent) :SelectId_(1), hJacketPict_(-1),level_(Normal)
{
	GameCsvReader musicState("Csv/MusicState.csv");
	MaxSelectId_ = musicState.GetLines() - 1;
}

SelectScene::~SelectScene()
{
}

void SelectScene::Initialize()
{
	pText = new Text;
	pText->Initialize();
	transform_.position_ = {};
	RefreshMusicData();
}
std::string GetLevelName(Level level)//レベルの文字列表記
{
	switch (level)
	{
	case Easy:
		return"Easy";
	case Normal:
		return"Normal";
	case Hard:
		return"Hard";
	default:
		return"";
	}
}


void SelectScene::Update()
{
	bool changed = false;
	if (Input::IsKeyDown(DIK_UP))
	{
		SelectId_++;
		if (SelectId_ > MaxSelectId_) {
			SelectId_ = 1;
		}
		changed = true;
	}
	if (Input::IsKeyDown(DIK_DOWN)) {
		SelectId_--;
		if (SelectId_ <= 0) {
			SelectId_ = MaxSelectId_;
		}
		changed = true;
	}
	if (Input::IsKeyDown(DIK_RIGHT)) {
		if (level_ == Hard) {
			level_ = Easy;
		}
		else {
			level_ = (Level)(level_ + 1);
		}
	}
	if (Input::IsKeyDown(DIK_LEFT)) {
		if (level_ == Easy) {
			level_ = Hard;
		}
		else {
			level_ = (Level)(level_ - 1);
		}
	}
	if (Input::IsKeyDown(DIK_SPACE)) {
		gSelectedMusicId = SelectId_;
		gSelectedMusicName = MusicName_;
		gSelectedMusicLevel = GetLevelName(level_);
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_PLAY);

	}
	if (changed) {
		RefreshMusicData();
	}
	
}

void SelectScene::Draw()
{
	std::string levelText = GetLevelName(level_);
	pText->Draw(30, 30, MusicName_.c_str());
	Image::SetTransform(hJacketPict_, transform_);
	Image::Draw(hJacketPict_);
	pText->Draw(50, 50, "SelectScene");
	pText->Draw(50, 70, levelText.c_str());

}

void SelectScene::Release()
{
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}
}

void SelectScene::RefreshMusicData()
{
	GameCsvReader musicState("Csv/MusicState.csv");

	MusicName_ = "";
	hJacketPict_ = -1;
	for (int i = 1;i < musicState.GetLines();i++) {
		int id = musicState.GetInt(i, 0);
		if (id == SelectId_) {
			MusicName_ = musicState.GetString(i, 1);
			JacketName_ = musicState.GetString(i, 2);
			break;
		}
	}
	if (JacketName_.empty()) {
		return;
	}

	std::string path = "Jacket/" + JacketName_ + ".png";
	hJacketPict_ = Image::Load(path.c_str());
}


