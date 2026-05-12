#pragma once
#include"NoteBase.h"
#include "Engine/GameObject.h"
#include"Player.h"
class BesideBeam :public NoteBase
{
public:
    //コンストラクタ
    BesideBeam(GameObject* parent);
    //デストラクタ
    ~BesideBeam()override;
    //初期化
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Release();
    void OnCollision(GameObject* pTarget)override;

private:
    int hBesideBeamModel_;
    bool PlayerHit = false;

};

