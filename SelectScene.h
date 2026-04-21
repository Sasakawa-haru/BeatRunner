#pragma once
#include"Engine/GameObject.h"
#include"Engine/Text.h"
#include<string>

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
	int SelectId_;
	int MaxSelectId_;
	int hJacketPict_;
	Text* pText;
};

