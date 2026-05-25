#include "ClearScene.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"
#include"Engine/Text.h"
#include<string>

ClearSene::ClearSene(GameObject* parent)
{
}

void ClearSene::Initialize()
{
	text_.Initialize();
}

void ClearSene::Update()
{
	if(Input::IsKeyDown(DIK_SPACE))
	{
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_SELECT);
	}
}

void ClearSene::Draw()
{
	text_.Draw(100,80,"Result");
}

void ClearSene::Release()
{
}
