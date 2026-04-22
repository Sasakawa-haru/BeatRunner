#pragma once
#include"Engine/GameObject.h"
#include"Engine/Text.h"
#include<string>
#include<vector>

class SelectScene:public GameObject
{
public:
	SelectScene(GameObject*parent);
	~SelectScene();
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void Release()override;

	void RefreshMusicData();

private:
	std::string MusicName_;
	std::string JacketName_;
	int SelectId_=1;
	int MaxSelectId_=0;
	int hJacketPict_=-1;
	Text* pText=nullptr;
};

