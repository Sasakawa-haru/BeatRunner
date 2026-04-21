#include "SelectScene.h"
#include"Music.h"
#include"Engine/Input.h"
#include"Engine/Image.h"
#include"Engine/Audio.h"
#include"Engine/Time.h"
#include"Engine/Text.h"
#include"Engine/GameCsvReader.h"


SelectScene::SelectScene(GameObject* parent) :SelectId_(1), hJacketPict_(-1)
{
	GameCsvReader musicState("Assets/Csv/MusicState.csv");
	MaxSelectId_ = musicState.GetLines() - 1;
}

SelectScene::~SelectScene()
{
}

void SelectScene::Initialize()
{
	pText = new Text;
	pText->Initialize();

	RefreshMusicData();
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
	if (changed) {
		RefreshMusicData();
	}
}

void SelectScene::Draw()
{
	pText->Draw(30, 30, MusicName_.c_str());
	Image::SetTransform(hJacketPict_, transform_);
	Image::Draw(hJacketPict_);

}

void SelectScene::Release()
{
}

void SelectScene::RefreshMusicData()
{
	GameCsvReader musicState("Assets/Csv/MusicState.csv");
	for (int i = 0;i < musicState.GetLines();i++) {
		int id = musicState.GetInt(i, 0);
		if (id == SelectId_) {
			MusicName_ = musicState.GetString(i, 1);
			break;
		}
	}
	std::string path = "Assets/Jacket/" + MusicName_ + ".png";
	hJacketPict_ = Image::Load(path.c_str());
}