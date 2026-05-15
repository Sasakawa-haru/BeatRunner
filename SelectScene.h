#pragma once
#include"Engine/GameObject.h"
#include"Engine/Text.h"
#include"Option.h"
#include<string>
#include<vector>

enum Level
{
	Easy,
	Normal,
	Hard
};

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
	Option* pOption_ = nullptr;
	std::string MusicName_;
	std::string JacketName_;
	int SelectId_=1;
	int MaxSelectId_=0;
	int hJacketPict_=-1;
	Text* pText=nullptr;
	Level level_;
};

