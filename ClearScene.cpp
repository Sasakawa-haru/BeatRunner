#include "ClearScene.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"

ClearSene::ClearSene(GameObject* parent)
{
}

void ClearSene::Initialize()
{
	
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
}

void ClearSene::Release()
{
}
