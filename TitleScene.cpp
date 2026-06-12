#include "TitleScene.h"
#include"Engine/Input.h"
#include "Engine/SceneManager.h"
#include"Engine/Image.h"
#include"JsonToCsvConverter.h"
//コンストラクタ
TitleScene::TitleScene(GameObject* parent)
	: GameObject(parent, "TitleScene"),
	hTitleImage_(-1)
{
}

//初期化
void TitleScene::Initialize()
{
	hTitleImage_ = Image::Load("Image/Title.png");
}

//更新
void TitleScene::Update()
{
	if (Input::IsKeyDown(DIK_SPACE)) {
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_SELECT);
	}
	if (Input::IsKeyDown(DIK_0)){
		ConvertAllJsonInDir_IfJsonNewer("Notes");
	}
	if (Input::IsKey(DIK_LALT) && Input::IsKey(DIK_F4)) {

	}
}

//描画
void TitleScene::Draw()
{
	Image::SetTransform(hTitleImage_, transform_);
	Image::Draw(hTitleImage_);
}

//開放
void TitleScene::Release()
{
}