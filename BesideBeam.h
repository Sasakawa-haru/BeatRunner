#pragma once
#include"NoteBase.h"
#include "Engine/GameObject.h"
class BesideBeam :public NoteBase
{
public:
    BesideBeam(GameObject* parent);
    ~BesideBeam()override;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Release()override;
private:
    int hBesideBeamModel_;

};

