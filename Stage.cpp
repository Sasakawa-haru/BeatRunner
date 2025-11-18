#include "Stage.h"
#include"Engine/Model.h"

int hStageModel_ = -1;

//コンストラクタ
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage")
{
}

//デストラクタ
Stage::~Stage()
{
}

//初期化
void Stage::Initialize()
{
    hStageModel_ = Model::Load("stage/stage.fbx");
}

//更新
void Stage::Update()
{
}

//描画
void Stage::Draw()
{
    Model::SetTransform(hStageModel_, transform_);
    Model::Draw(hStageModel_);
}

//開放
void Stage::Release()
{
}