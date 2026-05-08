#include "TitleScene.h"
#include"Engine/Input.h"
#include "Engine/SceneManager.h"
#include"JsonToCsvConverter.h"
//コンストラクタ
TitleScene::TitleScene(GameObject* parent)
	: GameObject(parent, "TitleScene")
{
}

//初期化
void TitleScene::Initialize()
{
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
}

//開放
void TitleScene::Release()
{
}